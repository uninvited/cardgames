#include "game.h"
#include "exception.h"
#include "logging/logging.h"
#include "serialize.h"
#include "utils.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <set>

namespace miplot::cardgame::durak {

namespace {

constexpr size_t NUM_INITIAL_CARDS = 6;
constexpr size_t MIN_PLAYERS = 2;
constexpr size_t MAX_PLAYERS = 5;

} // namespace

GameState::GameState(const Game& game)
    : game_(game)
{
    opponents_.reserve(game.players().size());
    for (const auto& player : game.players()) {
        opponents_.push_back(Opponent{player.name(), player.numCards()});
    }
}

const Opponents& GameState::opponents() const
{
    const auto& players = game_.players();
    for (size_t idx = 0; idx < players.size(); ++idx) {
        opponents_[idx].numCards = players[idx].numCards();
    }

    return opponents_;
}

Suit GameState::trumpSuit() const
{
    return game_.trumpSuit();
}

size_t GameState::mainAttackerIdx() const
{
    return game_.mainAttackerIdx();
}

size_t GameState::defenderIdx() const
{
    return game_.defenderIdx();
}

size_t GameState::curAttackerIdx() const
{
    return game_.curAttackerIdx();
}

const Cards& GameState::undefendedCards() const
{
    return game_.undefendedCards();
}

const CardPairs& GameState::defendedCards() const
{
    return game_.defendedCards();
}

const Cards& GameState::discard() const
{
    return game_.discard();
}


Game::Game(std::vector<Player>&& players)
    : players_(std::move(players))
    , deck_(Deck::create())
{
    REQUIRE(players_.size() >= MIN_PLAYERS && players_.size() <= MAX_PLAYERS,
            "Invalid number of players: " << players_.size());
}

RoundResult Game::playRound(size_t firstAttackerIdx)
{
    deal(firstAttackerIdx);
    INFO() << "Playing a round";
    printDeck();
    INFO() << "trump suit: " << trumpSuit_;

    BoutResult boutResult;

    while (!isFinished()) {
        boutResult = playBout();
        refill();

        if (!isFinished()) {
            shiftTurn(boutResult);
        }

    }

    RoundResult result = getRoundResult();

    cleanup();
    return result;
}

void Game::deal(size_t firstAttackerIdx)
{
    deck_.shuffle();
    for (auto& player : players_) {
        player.assignHand(deck_.getFromTop(NUM_INITIAL_CARDS));
    }
    trumpSuit_ = deck_.top().suit();
    deck_.putOnBottom(deck_.getOneFromTop());

    mainAttackerIdx_ = firstAttackerIdx;
    curAttackerIdx_ = firstAttackerIdx;
    defenderIdx_ = nextPlayerIdx(mainAttackerIdx_);

    state_ = std::make_unique<GameState>(*this);
}

BoutResult Game::playBout()
{
    bool resign = false;
    size_t numFolds = 0;

    DEBUG() << "Start bout, player " << curAttackerIdx_ << " to attack";
    printHands();

    while (numFolds < numPlayers() - 1
            && undefended_.size() < defender().numCards()
            && undefended_.size() + defended_.size() < NUM_INITIAL_CARDS)
    {
        // attack
        int attackIdx = -1;
        if (curAttacker().numCards() > 0) {
            attackIdx = curAttacker().attack(*state_);
            validateAttack(attackIdx);
        }
        if (attackIdx == -1) {
            DEBUG() << "Player " << curAttackerIdx_ << " folds";
            ++numFolds;
            curAttackerIdx_= nextAttackerIdx(curAttackerIdx_);
            continue;
        } else {
            DEBUG() << "Player " << curAttackerIdx_ << " attack: " << curAttacker().hand()[attackIdx];
            undefended_.push_back(curAttacker().playCard(attackIdx));
            numFolds = 0;
        }

        // defend
        if (!resign) {
            int defenseIdx = defender().defend(*state_);

            if (defenseIdx == -1) {
                DEBUG() << "Player " << defenderIdx_ << " resigns";
                resign = true;
            } else {
                validateDefense(defenseIdx);
                DEBUG() << "Player " << defenderIdx_ << " defense: " << defender().hand()[defenseIdx];
                defended_.push_back({std::move(undefended_[0]), defender().playCard(defenseIdx)});
                undefended_.clear();
            }
        }

        printTable();
    }

    if (resign) {
        resignPickup();
        return BoutResult::Resigned;
    } else {
        beatenDiscard();
        return BoutResult::Beaten;
    }
}

void Game::beatenDiscard()
{
    DEBUG() << "beatenDiscard";
    for (auto& pair : defended_) {
        discard_.push_back(std::move(pair.attacking));
        discard_.push_back(std::move(pair.defending));
    }
    undefended_.clear();
    defended_.clear();
}

void Game::resignPickup()
{
    DEBUG() << "resignPickup";
    defender().addToHand(std::move(undefended_));
    defender().addToHand(std::move(defended_));
    undefended_.clear();
    defended_.clear();
}

void Game::refill()
{
    DEBUG() << "refill";
    for (size_t i = 0, idx = mainAttackerIdx_;
            i < numPlayers() && !deck_.isEmpty();
            ++i, idx = nextPlayerIdx(idx))
    {
        Player& player = players_[idx];
        if (player.numCards() >= NUM_INITIAL_CARDS) {
            continue;
        }

        size_t n = std::min(NUM_INITIAL_CARDS - player.numCards(), deck_.size());
        player.addToHand(deck_.getFromTop(n));
    }
}

void Game::shiftTurn(BoutResult prevBoutResult)
{
    DEBUG() << "shiftTurn";
    mainAttackerIdx_ = prevBoutResult == BoutResult::Resigned
                     ? nextPlayerWithCardsIdx(defenderIdx_)
                     : defenderIdx_;
    curAttackerIdx_ = mainAttackerIdx_;
    defenderIdx_ = nextPlayerWithCardsIdx(mainAttackerIdx_);
}

void Game::cleanup()
{
    DEBUG() << "cleanup";
    // Discard all cards and put them back to the deck
    for (auto& player : players_) {
        deck_.putOnTop(player.discardHand());
    }
    deck_.putOnTop(std::move(discard_));
    discard_.clear();
}


void Game::validateAttack(int cardIdx) const
{
    REQUIRE(cardIdx < (int)curAttacker().numCards(),
            "Invalid attacking card index: " << cardIdx);

    if (defended_.empty() && undefended_.empty()) {
        // Initial attack
        REQUIRE(cardIdx > -1, "Empty initial attack");
        return;
    } else if (cardIdx == -1) {
        return;
    }

    const auto& card = curAttacker().hand()[cardIdx];

    bool isOneOfDefended = std::any_of(defended_.begin(), defended_.end(),
        [&](const CardPair& p) {
            return p.attacking.rank() == card.rank()
                || p.defending.rank() == card.rank();
            });
    bool isOnOfUndefended = std::any_of(undefended_.begin(), undefended_.end(),
        [&](const Card& c) { return c.rank() == card.rank(); });

    REQUIRE(isOneOfDefended || isOnOfUndefended,
            "Attacking with a rank not seen before: " << card);

    REQUIRE(undefended_.size() + 1 <= defender().numCards(),
            "Attacking with more cards than defender has");
    REQUIRE(undefended_.size() + defended_.size() + 1 <= NUM_INITIAL_CARDS,
            "Attacking with more than maximum allowed cards");
}

void Game::validateDefense(int cardIdx) const
{
    if (cardIdx == -1) {
        return;
    }

    REQUIRE(cardIdx < (int)defender().numCards(),
            "Invalid defending card index: " << cardIdx);

    const auto& card = defender().hand()[cardIdx];
    const auto& attacker = undefended_.front();
    REQUIRE((attacker.suit() == card.suit() && attacker.rank() < card.rank()) ||
            (attacker.suit() != card.suit() && card.suit() == trumpSuit_),
            "Invalid defense of " << attacker << " by " << card);
}

size_t Game::nextPlayerIdx(size_t playerIdx) const
{
    return (playerIdx + 1) % players_.size();
}

size_t Game::nextPlayerWithCardsIdx(size_t playerIdx) const
{
    do {
        playerIdx = (playerIdx + 1) % players_.size();
    } while (!players_[playerIdx].numCards());
    return playerIdx;
}

size_t Game::nextAttackerIdx(size_t playerIdx) const
{
    do {
        playerIdx = (playerIdx + 1) % players_.size();
    } while (playerIdx != defenderIdx_);
    return playerIdx;
}

bool Game::isFinished() const
{
    auto numActivePlayers = std::count_if(players_.begin(), players_.end(),
        [](const Player& p) { return p.numCards() > 0; });
    return numActivePlayers < 2;
}

RoundResult Game::getRoundResult() const
{
    auto itr = std::find_if(players_.begin(), players_.end(),
        [](const Player& p) { return p.numCards() > 0; });

    RoundResult result;
    if (itr != players_.end()) {
        result.losingPlayerIdx = std::distance(players_.begin(), itr);
    } else {
        result.losingPlayerIdx = std::nullopt;
    }
    return result;
}


void Game::printDeck() const
{
    DEBUG() << "Deck: {" << join(deck_.cards()) << "}";
}

void Game::printHands() const
{
    for (size_t i = 0; i < players_.size(); ++i) {
        const auto& p = players_[i];
        DEBUG() << "Player " << i << " hand: {" << join(p.hand()) << "}";
    }
}

void Game::printTable() const
{
    DEBUG() << "Undefended: {" << join(undefended_) << "}"
            << ". Defended: {" << join(defended_) << "}";
}

void Game::printDiscard() const
{
    DEBUG() << "Discard: {" << join(discard_) << "}";
}

} // namespace miplot::cardgame::durak


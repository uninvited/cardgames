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
    auto card = deck_.getOneFromTop();
    trumpSuit_ = card.suit();
    deck_.putOnBottom(std::move(card));

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
        Cards attack;
        size_t numCards = curAttacker().numCards();
        if (numCards > 0) {
            attack = curAttacker().attack(*state_);
            validateAttack(attack, numCards);
        }
        if (attack.empty()) {
            DEBUG() << "Player " << curAttackerIdx_ << " folds";
            ++numFolds;
            curAttackerIdx_= nextAttackerIdx(curAttackerIdx_);
            continue;
        } else {
            DEBUG() << "Player " << curAttackerIdx_ << " attack: " << join(attack);
            std::move(attack.begin(), attack.end(), std::back_inserter(undefended_));
            numFolds = 0;
        }

        // defend
        if (!resign) {
            numCards = defender().numCards();
            auto defense = defender().defend(*state_);

            if (defense.empty()) {
                DEBUG() << "Player " << defenderIdx_ << " resigns";
                resign = true;
            } else {
                DEBUG() << "Player " << defenderIdx_ << " defense: " << join(defense);
                validateDefense(defense, numCards);
                for (size_t i = 0; i < defense.size(); ++i) {
                    defended_.push_back({std::move(undefended_[i]), std::move(defense[i])});
                }
                undefended_.clear();
            }
        }
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


void Game::validateAttack(const Cards& cards, size_t initialNumCards) const
{
    REQUIRE(std::all_of(cards.begin(), cards.end(),
                [&](const Card& c){ return c.deckId() == deck_.deckId(); }),
            "Card from a wrong deck");
    REQUIRE(curAttacker().numCards() == initialNumCards - cards.size(),
            "Wrong number of cards in attacker's hand");

    if (defended_.empty() && undefended_.empty()) {
        // Initial attack
        REQUIRE(!cards.empty(), "Empty attack");
        REQUIRE(std::all_of(cards.begin(), cards.end(),
                    [&](const Card& c){ return c.rank() == cards[0].rank(); }),
                "Initial attack must include cards of the same rank");
    } else {
        // Follow-up attack
        for (const auto& card : cards) {
            bool isOneOfDefended = std::any_of(defended_.begin(), defended_.end(),
                [&](const CardPair& p) {
                    return p.attacking.rank() == card.rank()
                        || p.defending.rank() == card.rank();
                });
            bool isOnOfUndefended = std::any_of(undefended_.begin(), undefended_.end(),
                [&](const Card& c) { return c.rank() == card.rank(); });

            REQUIRE(isOneOfDefended || isOnOfUndefended,
                    "Attacking with a rank not seen before: " << card);
        }
    }

    DEBUG() << "Attacking with: " << join(cards);
    printTable();
    DEBUG() << "Defender hand: " << join(defender().hand());

    REQUIRE(cards.size() + undefended_.size() <= defender().numCards(),
            "Attacking with more cards than defender has");
    REQUIRE(cards.size() + undefended_.size() + defended_.size() <= NUM_INITIAL_CARDS,
            "Attacking with more than maximum allowed cards");
}

void Game::validateDefense(const Cards& cards, size_t initialNumCards) const
{
    if (cards.empty())
        return;

    REQUIRE(std::all_of(cards.begin(), cards.end(),
                [&](const Card& c){ return c.deckId() == deck_.deckId(); }),
            "Card from a wrong deck");
    REQUIRE(defender().numCards() == initialNumCards - cards.size(),
            "Wrong number of cards in defender's hand");

    REQUIRE(cards.size() == undefended_.size(),
            "Don't have enough cards to defend");

    // Check that defender is greater than attacker
    for (size_t i = 0; i < cards.size(); ++i) {
        const auto& attacker = undefended_[i];
        const auto& defender = cards[i];
        if (attacker.suit() == defender.suit()) {
            REQUIRE(attacker.rank() < defender.rank(),
                    "Invalid defense of " << attacker << " by " << defender);
        } else {
            REQUIRE(defender.suit() == trumpSuit_,
                    "Invalid defense of " << attacker << " by " << defender);
        }
    }
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


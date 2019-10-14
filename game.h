#pragma once

#include "card.h"
#include "deck.h"
#include "player.h"

#include <memory>
#include <optional>
#include <unordered_map>

namespace miplot::cardgame::durak {

// Player seen by another player
struct Opponent {
    PlayerId playerId;
    size_t numCards;
};

using Opponents = std::vector<Opponent>;

enum class BoutResult { Beaten, Resigned };

struct RoundResult {
    std::optional<size_t> losingPlayerIdx;
};

class Game;

// Game state seen by a player
class GameState {
public:
    GameState(const Game& game);

    const Opponents& opponents() const;

    Suit trump() const;

    // Indices in opponents() container
    size_t mainAttackerIdx() const;
    size_t defenderIdx() const;
    size_t curAttackerIdx() const;

    // Cards on the table
    const Cards& undefendedCards() const;
    const CardPairs& defendedCards() const;

    // Cards in discard heap
    const Cards& discard() const;

private:
    const Game& game_;
    mutable Opponents opponents_;
};

class Game {
public:
    Game(Players&& players);

    RoundResult playRound(size_t firstAttackerIdx);

    const Players& players() const { return players_; }
    size_t numPlayers() const { return players_.size(); }

    // Bottom card of the deck
    Suit trump() const { return trump_; }

    size_t mainAttackerIdx() const { return mainAttackerIdx_; }
    size_t curAttackerIdx() const {return curAttackerIdx_; }
    size_t defenderIdx() const { return defenderIdx_; }

    const Cards& undefendedCards() const { return undefended_; }
    const CardPairs& defendedCards() const { return defended_; }

    const Cards& discard() const { return discard_; }

private:
    void deal(size_t firstAttackerIdx);

    BoutResult playBout();

    // Move all defended cards to discard
    void beatenDiscard();
    // Defender picks up all cards from the table
    void resignPickup();

    // Refill players hands from the deck at the end of turn
    void refill();

    // Set next attacker and defender
    void shiftTurn(BoutResult prevBoutResult);

    // Restore the deck
    void cleanup();


    void validateAttack(const Cards& cards, size_t initialNumCards) const;
    void validateDefense(const Cards& cards, size_t initialNumCards) const;

    size_t nextPlayerIdx(size_t playerIdx) const;
    size_t nextPlayerWithCardsIdx(size_t playerIdx) const;
    size_t nextAttackerIdx(size_t playerIdx) const;
    bool isFinished() const;
    RoundResult getRoundResult() const;

    // helpers
    const Player& mainAttacker() const { return players_[mainAttackerIdx_]; }
    const Player& curAttacker() const { return players_[curAttackerIdx_]; }
    const Player& defender() const { return players_[defenderIdx_]; }
    Player& mainAttacker() { return players_[mainAttackerIdx_]; }
    Player& curAttacker() { return players_[curAttackerIdx_]; }
    Player& defender() { return players_[defenderIdx_]; }

    // Logging
    void printDeck() const;
    void printHands() const;
    void printTable() const;
    void printDiscard() const;

private:
    Players players_;

    Suit trump_;

    size_t mainAttackerIdx_;
    size_t curAttackerIdx_;
    size_t defenderIdx_;

    Cards undefended_;
    CardPairs defended_;

    Deck deck_;

    Cards discard_;

    std::unique_ptr<GameState> state_;

    // todo: total score of all rounds?
};

} // namespace miplot::cardgame::durak

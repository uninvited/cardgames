#pragma once

#include "card.h"

#include <random>
#include <set>
#include <string>

namespace miplot::cardgame::durak {

class GameState;

class Strategy {
public:

    /**
     * @param state game state
     * @param hand player's hand.
     * @return index of card in hand to attack with. -1 if folds
     */
    virtual int attack(const GameState& state, const Cards& hand) = 0;

    /**
     * @param state game state
     * @param hand player's hand.
     * @return index of card in hand to defend with. -1 if resigns
     */
    virtual int defend(const GameState& state, const Cards& hand) = 0;
};


class RandomStrategy : public Strategy {
public:
    RandomStrategy();

    int attack(const GameState& state, const Cards& hand) override;

    int defend(const GameState& state, const Cards& hand) override;

private:
    std::mt19937 randGenerator_;
};

} // namespace miplot::cardgame::durak


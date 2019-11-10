#pragma once

#include "card.h"

#include <random>
#include <set>
#include <string>

namespace miplot::cardgame::durak {

constexpr size_t MAX_ATTACK_SIZE = 6;

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

    virtual const std::string& name() const {
        static const std::string NAME = "Noname strategy";
        return NAME;
    }
};


class RandomStrategy : public Strategy {
public:
    RandomStrategy();

    int attack(const GameState& state, const Cards& hand) override;

    int defend(const GameState& state, const Cards& hand) override;

    const std::string& name() const override;
private:
    std::mt19937 randGenerator_;
};

class MinCardStrategy : public Strategy {
    int attack(const GameState& state, const Cards& hand) override;

    int defend(const GameState& state, const Cards& hand) override;

    const std::string& name() const override;
};

} // namespace miplot::cardgame::durak


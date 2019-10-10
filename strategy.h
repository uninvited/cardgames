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
     * @param hand player's hand. It must be updated accordingly, i.e.
     *             attacking cards must be removed from the hand
     *             by the moment the function returns
     * @return cards to attack with
     */
    virtual Cards attack(const GameState& state, Cards& hand) = 0;

    /**
     * @param state game state
     * @param hand player's hand. It must be updated accordingly, i.e.
     *             defending cards must be removed from the hand
     *             by the moment the function returns
     * @return cards to defend, their order must correspond to the order
     *         of state.undefendedCards()
     */
    virtual Cards defend(const GameState& state, Cards& hand) = 0;
};


class RandomStrategy : public Strategy {
public:
    RandomStrategy();

    Cards attack(const GameState& state, Cards& hand) override;

    Cards defend(const GameState& state, Cards& hand) override;
private:
    // -1 if can't defend
    int getDefenderIdx(const Card& attacker,
                       const Cards& hand,
                       const std::vector<bool>& used,
                       Suit trump) const;

    std::mt19937 randGenerator_;
};

} // namespace miplot::cardgame::durak


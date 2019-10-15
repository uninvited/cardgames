#pragma once

#include "card.h"
#include "strategy.h"

#include <string>
#include <memory>


namespace miplot::cardgame::durak {

using PlayerId = std::string;

class GameState;


class Player {
public:
    Player(PlayerId name, std::unique_ptr<Strategy>&& strategy);

    const PlayerId name() const;

    const Cards& hand() const;

    size_t numCards() const;

    void assignHand(Cards&& cards);
    void addToHand(Cards&& cards);
    void addToHand(CardPairs&& pairs);

    Card playCard(size_t cardIdx);

    Cards discardHand();

    // Return index of card in hand, or -1 on fold
    int attack(const GameState& state);

    // Return index of card in hand, or -1 on resign
    int defend(const GameState& state);

private:
    PlayerId name_;
    std::unique_ptr<Strategy> strategy_;
    Cards hand_;
};

using Players = std::vector<Player>;

} // namespace miplot::cardgame::durak

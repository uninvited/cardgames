#pragma once

#include "common/deck.h"
#include "card.h"

#include <random>

namespace miplot::cardgame::durak {
/*
class Deck {
public:
    // Creates a deck with 36 cards
    Deck();

    const Cards& cards() const;

    size_t size() const;

    bool isEmpty() const;

    Card getOneFromTop();

    Cards getFromTop(size_t count);

    void putOnTop(Cards cards);

    void shuffle();

    static constexpr size_t NUM_CARDS = cards::Std36CardTraits::radix();
private:
    Cards cards_;
    std::mt19937 randGenerator_;
};
*/
using Deck = cards::Deck36;

} // namespace miplot::cardgame::durak

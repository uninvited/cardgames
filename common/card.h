#pragma once

#include "card_traits.h"

#include <ostream>

namespace miplot::cards {

template <typename CardTraits>
class Deck;

/**
 * Card is intentinally noncopyable to prevent users from cheating
 */
template <typename CardTraits>
class Card {
public:
    using Suit = typename CardTraits::SuitType;
    using Rank = typename CardTraits::RankType;

    Card(Suit suit, Rank rank)
        : suit_(suit)
        , rank_(rank)
    {}

    Card(Card&&) = default;
    Card& operator= (Card&&) = default;

    Suit suit() const { return suit_; }
    Rank rank() const { return rank_; }

private:
    template<typename T> friend class Deck;

    Suit suit_;
    Rank rank_;
};


template <typename CardTraits>
std::ostream& operator<< (std::ostream& os, const Card<CardTraits>& card)
{
    return os << CardTraits::toString(card.rank())
              << CardTraits::toString(card.suit());
}

using Card36 = Card<Std36CardTraits>;

} // namespace miplot::cards

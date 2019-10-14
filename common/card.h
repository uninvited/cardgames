#pragma once

#include "card_traits.h"

#include <ostream>

namespace miplot::cards {

using DeckId = uint64_t;

template <typename CardTraits, template <typename> class Cont>
class Deck;

/**
 * Card is intentinally noncopyable to prevent users from cheating
 */
template <typename CardTraits>
class Card {
public:
    using Suit = typename CardTraits::SuitType;
    using Rank = typename CardTraits::RankType;

    Card(Card&&) = default;
    Card& operator= (Card&&) = default;

    Suit suit() const { return suit_; }
    Rank rank() const { return rank_; }
    DeckId deckId() const { return deckId_; }

private:
    template<typename T, template <typename> class Cont>
    friend class Deck;

    Card(Suit suit, Rank rank, DeckId deckId)
        : suit_(suit)
        , rank_(rank)
        , deckId_(deckId)
    {}

    Suit suit_;
    Rank rank_;
    DeckId deckId_;
};


template <typename CardTraits>
std::ostream& operator<< (std::ostream& os, const Card<CardTraits>& card)
{
    return os << CardTraits::toString(card.rank())
              << CardTraits::toString(card.suit());
}

using Card36 = Card<Std36CardTraits>;

} // namespace miplot::cards

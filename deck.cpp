#include "deck.h"
#include "exception.h"

#include <algorithm>
#include <iterator>
#include <random>

#include <iostream>

namespace miplot::cardgame::durak {

namespace {

const Suit ALL_SUITS[] = {Suit::Clubs, Suit::Diamonds, Suit::Hearts, Suit::Spades};

const Rank ALL_RANKS[] = {Rank::Six, Rank::Seven, Rank::Eight, Rank::Nine, Rank::Ten,
                          Rank::Jack, Rank::Queen, Rank::King, Rank::Ace};

} // namespace
/*
Deck::Deck()
    : randGenerator_(std::random_device{}())
{
    cards_.reserve(NUM_CARDS);
    for (Suit suit : ALL_SUITS) {
        for (Rank rank : ALL_RANKS) {
            cards_.push_back(Card{suit, rank});
        }
    }
}

const Cards& Deck::cards() const
{
    return cards_;
}

size_t Deck::size() const
{
    return cards_.size();
}

bool Deck::isEmpty() const
{
    return cards_.empty();
}

Card Deck::getOneFromTop()
{
    REQUIRE(!isEmpty(), "Not enough cards");
    Card result = cards_.back();
    cards_.pop_back();
    return result;
}

Cards Deck::getFromTop(size_t count)
{
    REQUIRE(count <= size(), "Not enough cards");
    Cards result(count);
    std::move(cards_.end() - count, cards_.end(), result.begin());
    cards_.resize(size() - count);
    return result;
}

void Deck::putOnTop(Cards cards)
{
    std::move(cards.begin(), cards.end(), std::back_inserter(cards_));
}

void Deck::shuffle()
{
    std::shuffle(cards_.begin(), cards_.end(), randGenerator_);
}

*/

} // namespace miplot::cardgame::durak

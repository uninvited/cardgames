#pragma once

#include "card.h"
#include "enum_iterator.h"
#include "exception.h"

#include <deque>
#include <ostream>
#include <random>
#include <tuple>
#include <vector>

namespace miplot::cards {

template <typename CardTraits>
class Deck {
public:
    using SuitType = typename CardTraits::SuitType;
    using RankType = typename CardTraits::RankType;
    using CardType = Card<CardTraits>;
    using ContainerType = std::deque<CardType>;
    using SuitIterator = EnumIterator<SuitType, CardTraits::minSuit(), CardTraits::maxSuit()>;
    using RankIterator = EnumIterator<RankType, CardTraits::minRank(), CardTraits::maxRank()>;

    static constexpr size_t RADIX = CardTraits::radix();

    // Creates an empty deck
    Deck()
        : randGenerator_(std::random_device{}())
    {
    }

    Deck(std::vector<CardType>&& cards)
        : randGenerator_(std::random_device{}())
    {
        std::move(cards.begin(), cards.end(), std::back_inserter(cards_));
    }

    // Creates standard deck with each card taken once
    static Deck create() {
        Deck deck;

        for (auto suit : SuitIterator()) {
            for (auto rank : RankIterator()) {
                deck.cards_.push_back(CardType(suit, rank));
            }
        }
        return deck;
    }

    const ContainerType& cards() const { return cards_; }

    size_t size() const { return cards_.size(); }

    bool isEmpty() const { return cards_.empty(); }

    const CardType& top() const
    {
        REQUIRE(!isEmpty(), "Not enough cards");
        return cards_.front();
    }

    const CardType& bottom() const
    {
        REQUIRE(!isEmpty(), "Not enough cards");
        return cards_.back();
    }

    CardType getOneFromTop()
    {
        REQUIRE(!isEmpty(), "Not enough cards");
        CardType result = std::move(cards_.front());
        cards_.pop_front();
        return result;
    }
    std::vector<CardType> getFromTop(size_t count)
    {
        REQUIRE(count <= size(), "Not enough cards");
        std::vector<CardType> result;
        result.reserve(count);
        auto till = cards_.begin() + count;
        std::move(cards_.begin(), till, std::back_inserter(result));
        cards_.erase(cards_.begin(), till);
        return result;
    }

    CardType getOneFromBottom()
    {
        REQUIRE(!isEmpty(), "Not enough cards");
        CardType result = std::move(cards_.back());
        cards_.pop_back();
        return result;
    }

    std::vector<CardType> getFromBottom(size_t count)
    {
        REQUIRE(count <= size(), "Not enough cards");
        std::vector<CardType> result;
        auto from = cards_.end() - count;
        std::move(from, cards_.end(), std::back_inserter(result));
        cards_.erase(from, cards_.end());
        return result;
    }

    void putOnTop(CardType card)
    {
        cards_.push_front(std::move(card));
    }

    template<typename Collection>
    void putOnTop(Collection cards)
    {
        std::move(cards.begin(), cards.end(), std::front_inserter(cards_));
    }

    void putOnBottom(CardType card)
    {
        cards_.push_back(std::move(card));
    }

    template<typename Collection>
    void putOnBottom(Collection cards)
    {
        std::move(cards.begin(), cards.end(), std::back_inserter(cards_));
    }

    void shuffle()
    {
        std::shuffle(cards_.begin(), cards_.end(), randGenerator_);
    }

private:
    ContainerType cards_;
    std::mt19937 randGenerator_;
};

using Deck36 = Deck<Std36CardTraits>;


} // namespace miplot::cards

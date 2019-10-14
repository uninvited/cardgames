#pragma once

#include "card.h"
#include "enum_iterator.h"
#include "id_generator.h"
#include "../exception.h"

#include <deque>
#include <ostream>
#include <random>
#include <tuple>
#include <vector>

namespace miplot::cards {

inline auto& deckIdGenerator()
{
    static IdGenerator<DeckId> generator_{0};
    return generator_;
}

template <typename CardTraits, template <typename> class Cont = std::deque>
class Deck {
public:
    using SuitType = typename CardTraits::SuitType;
    using RankType = typename CardTraits::RankType;
    using CardType = Card<CardTraits>;
    using ContainerType = Cont<CardType>;
    using SuitIterator = EnumIterator<SuitType, CardTraits::minSuit(), CardTraits::maxSuit()>;
    using RankIterator = EnumIterator<RankType, CardTraits::minRank(), CardTraits::maxRank()>;

    static constexpr size_t RADIX = CardTraits::radix();

    // Creates an empty deck
    Deck()
        : deckId_(deckIdGenerator().nextId())
        , randGenerator_(std::random_device{}())
    {
    }

    Deck(std::vector<CardType>&& cards)
        : deckId_(deckIdGenerator().nextId())
        , randGenerator_(std::random_device{}())
    {
        std::move(cards.begin(), cards.end(), std::back_inserter(cards_));
        for (auto& card : cards_) {
            card.deckId_ = deckId_;
        }
    }

    static Deck create() {
        Deck deck;
        if constexpr (std::is_same_v<ContainerType, std::vector<CardType>>) {
            deck.cards_.reserve(Deck::RADIX);
        }

        for (auto suit : SuitIterator()) {
            for (auto rank : RankIterator()) {
                deck.cards_.push_back(CardType(suit, rank, deck.deckId()));
            }
        }
        return deck;
    }

    DeckId deckId() const { return deckId_; }

    const ContainerType& cards() const { return cards_; }

    size_t size() const { return cards_.size(); }

    bool isEmpty() const { return cards_.empty(); }

    CardType getOneFromTop()
    {
        REQUIRE(!isEmpty(), "Not enough cards");
        CardType result = std::move(cards_.back());
        cards_.pop_back();
        return result;
    }
    std::vector<CardType> getFromTop(size_t count)
    {
        REQUIRE(count <= size(), "Not enough cards");
        std::vector<CardType> result;
        auto from = cards_.end() - count;
        std::move(from, cards_.end(), std::back_inserter(result));
        cards_.erase(from, cards_.end());
        return result;
    }

    CardType getOneFromBottom()
    {
        REQUIRE(!isEmpty(), "Not enough cards");
        CardType result = std::move(cards_.front());
        cards_.pop_front();
        return result;
    }

    std::vector<CardType> getFromBottom(size_t count)
    {
        REQUIRE(count <= size(), "Not enough cards");
        std::vector<CardType> result(count);
        auto till = cards_.begin() + count;
        std::move(cards_.begin(), till, result.begin());
        cards_.erase(cards_.begin(), till);
        return result;
    }

    template<typename T>
    void putOnTop(T cards)
    {
        std::move(cards.begin(), cards.end(), std::back_inserter(cards_));
    }

    template<typename T>
    void putOnBottom(T cards)
    {
        std::move(cards.begin(), cards.end(), std::front_inserter(cards_));
    }

    void shuffle()
    {
        std::shuffle(cards_.begin(), cards_.end(), randGenerator_);
    }

private:
    ContainerType cards_;
    DeckId deckId_;
    std::mt19937 randGenerator_;
};

using Deck36 = Deck<Std36CardTraits, std::deque>;


} // namespace miplot::cards

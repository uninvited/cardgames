#pragma once

#include "card.h"

namespace miplot::cardgame::durak {

bool less(const Card& lhs, const Card& rhs, Suit trump);

bool canDefend(const Card& attack, const Card& defense, Suit trump);

struct CardComparator {
public:
    CardComparator(Suit trump) : trump_(trump)
    {}

    bool operator()(const Card& lhs, const Card& rhs) const
    {
        return less(lhs, rhs, trump_);
    }
private:
    Suit trump_;
};

} // namespace miplot::cardgame::durak


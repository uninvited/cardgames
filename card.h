#pragma once

#include "common/card.h"
#include "common/card_traits.h"

#include <ostream>
#include <tuple>
#include <vector>

namespace miplot::cardgame::durak {

using Suit = cards::Suit4;
using Rank = cards::Rank9;
using Card = cards::Card36;
using Cards = std::vector<Card>;

struct CardPair {
    Card attacking;
    Card defending;
};

using CardPairs = std::vector<CardPair>;

} // namespace miplot::cardgame::durak

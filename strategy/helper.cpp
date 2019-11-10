#include "helper.h"

namespace miplot::cardgame::durak {

bool less(const Card& lhs, const Card& rhs, Suit trump)
{
    if (lhs.suit() == rhs.suit()) {
        return lhs.rank() < rhs.rank();
    }

    if (rhs.suit() == trump) {
        return true;
    }

    if (lhs.suit() == trump) {
        return false;
    }

    return lhs.rank() < rhs.rank();
}

bool canDefend(const Card& attack, const Card& defense, Suit trump)
{
    return
        (attack.suit() == defense.suit() && attack.rank() < defense.rank())
        ||
        (attack.suit() != defense.suit() && defense.suit() == trump);
}

} // namespace miplot::cardgame::durak



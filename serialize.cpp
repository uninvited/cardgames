#include "serialize.h"

namespace miplot::cardgame::durak {

std::ostream& operator<< (std::ostream& os, const CardPair& pair)
{
    return os << "(" << pair.attacking << "," << pair.defending << ")";
}

} // namespace miplot::cardgame::durak


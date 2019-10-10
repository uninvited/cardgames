#include "card_traits.h"

namespace miplot::cards {

std::ostream& operator<< (std::ostream& os, Suit4 suit)
{
    switch (suit) {
        case Suit4::Clubs: return os << "\u2663";
        case Suit4::Diamonds: return os << "\u2666";
        case Suit4::Hearts: return os << "\u2665";
        case Suit4::Spades: return os << "\u2660";
    }
    return os << "[invalid]";
}

std::ostream& operator<< (std::ostream& os, Rank9 rank)
{
    switch (rank) {
        case Rank9::Six: return os << "6";
        case Rank9::Seven: return os << "7";
        case Rank9::Eight: return os << "8";
        case Rank9::Nine: return os << "9";
        case Rank9::Ten: return os << "10";
        case Rank9::Jack: return os << "J";
        case Rank9::Queen: return os << "Q";
        case Rank9::King: return os << "K";
        case Rank9::Ace: return os << "A";
    }
    return os << "[invalid]";
}

} // namespace miplot::cards


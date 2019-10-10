#pragma once

#include <string>
#include <iostream>
#include <sstream>

namespace miplot::cards {

template <typename S, typename R>
struct CardTraits {
    using SuitType = S;
    using RankType = R;

    static constexpr SuitType minSuit();
    static constexpr SuitType maxSuit();
    static constexpr RankType minRank();
    static constexpr RankType maxRank();

    static constexpr size_t numSuits();
    static constexpr size_t numRanks();
    static constexpr size_t radix();

    static std::string toString(SuitType s);
    static std::string toString(RankType s);
};

enum class Suit4 { Clubs, Diamonds, Hearts, Spades };
std::ostream& operator<< (std::ostream& os, Suit4 suit);

enum class Rank9 { Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };
std::ostream& operator<< (std::ostream& os, Rank9 rank);

struct Std36CardTraits {
    using SuitType = Suit4;
    using RankType = Rank9;

    static constexpr SuitType minSuit() { return SuitType::Clubs; }
    static constexpr SuitType maxSuit() { return SuitType::Spades; }
    static constexpr RankType minRank() { return RankType::Six; }
    static constexpr RankType maxRank() { return RankType::Ace; }

    static constexpr size_t numSuits() { return 4; }
    static constexpr size_t numRanks() { return 9; }
    static constexpr size_t radix() { return 36; }

    friend std::ostream& operator<<(std::ostream& os, Suit4 suit);
    friend std::ostream& operator<<(std::ostream& os, Rank9 rank);

    static std::string toString(SuitType suit)
    {
        std::ostringstream ss;
        ss << suit;
        return ss.str();
    }

    static std::string toString(RankType rank)
    {
        std::ostringstream ss;
        ss << rank;
        return ss.str();
    }
};

} // namespace miplot::cards

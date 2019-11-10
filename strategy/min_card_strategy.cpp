#include "strategy.h"
#include "helper.h"
#include "game.h"
#include "logging/logging.h"

#include <algorithm>
#include <random>

namespace miplot::cardgame::durak {

int MinCardStrategy::attack(const GameState& state, const Cards& hand)
{
    if (hand.empty()) {
        return -1;
    }

    if (state.defendedCards().empty() && state.undefendedCards().empty()) {
        auto itr = std::min_element(hand.begin(), hand.end(),
                                    CardComparator(state.trumpSuit()));
        return std::distance(hand.begin(), itr);
    } else {
        // Safety check
        if (state.defendedCards().size() + state.undefendedCards().size() >= MAX_ATTACK_SIZE
                || state.undefendedCards().size() >= state.opponents()[state.defenderIdx()].numCards) {
            return -1;
        }

        std::vector<int> candidates;
        for (size_t i = 0; i < hand.size(); ++i) {
            if (std::any_of(state.defendedCards().begin(),
                            state.defendedCards().end(),
                            [&](const CardPair& pair){
                                return pair.attacking.rank() == hand[i].rank()
                                    || pair.defending.rank() == hand[i].rank();
                            }))
            {
                candidates.push_back(i);
            }

            if (std::any_of(state.undefendedCards().begin(),
                            state.undefendedCards().end(),
                            [&](const Card& c){
                                return c.rank() == hand[i].rank()
                                    || c.rank() == hand[i].rank();
                            }))
            {
                candidates.push_back(i);
            }
        }

        CardComparator cmp(state.trumpSuit());
        auto itr = std::min_element(candidates.begin(), candidates.end(),
                [&](int lhsInd, int rhsInd) {
                    return cmp(hand[lhsInd], hand[rhsInd]);
                });

        return itr == candidates.end() ? -1 : *itr;
    }
}

int MinCardStrategy::defend(const GameState& state, const Cards& hand)
{
    if (hand.empty()) {
        return -1;
    }

    const auto& attacker = state.undefendedCards().front();

    std::vector<int> candidates;

    for (size_t i = 0; i < hand.size(); ++i) {
        const auto& card = hand[i];
        if (canDefend(attacker, card, state.trumpSuit())) {
            candidates.push_back(i);
        }
    }

    CardComparator cmp(state.trumpSuit());
    auto itr = std::min_element(candidates.begin(), candidates.end(),
            [&](int lhsInd, int rhsInd) {
                return cmp(hand[lhsInd], hand[rhsInd]);
            });

    return itr == candidates.end() ? -1 : *itr;
}

const std::string& MinCardStrategy::name() const
{
    static const std::string NAME = "Minimal card strategy";
    return NAME;
}

} // namespace miplot::cardgame::durak


#include "strategy.h"
#include "helper.h"
#include "game.h"
#include "logging/logging.h"

#include <algorithm>
#include <random>

namespace miplot::cardgame::durak {

RandomStrategy::RandomStrategy()
    : randGenerator_(std::random_device{}())
{
}

int RandomStrategy::attack(const GameState& state, const Cards& hand)
{
    if (hand.empty()) {
        return -1;
    }

    if (state.defendedCards().empty() && state.undefendedCards().empty()) {
        // Initial attack
        return randGenerator_() % hand.size();
    } else {
        // Follow-up attack

        // Safety check
        if (state.defendedCards().size() + state.undefendedCards().size() >= MAX_ATTACK_SIZE
                || state.undefendedCards().size() >= state.opponents()[state.defenderIdx()].numCards) {
            return -1;
        }

        // Use -1(=fold) as one of random options
        std::vector<int> candidates{-1};
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
        size_t index = randGenerator_() % candidates.size();
        return candidates[index];
    }
}

int RandomStrategy::defend(const GameState& state, const Cards& hand)
{
    if (hand.empty()) {
        return -1;
    }

    const auto& attacker = state.undefendedCards().front();

    // Use -1(=resign) as one of random options
    std::vector<int> candidates{-1};

    for (size_t i = 0; i < hand.size(); ++i) {
        const auto& card = hand[i];
        if (canDefend(attacker, card, state.trumpSuit())) {
            candidates.push_back(i);
        }
    }
    size_t index = randGenerator_() % candidates.size();
    return candidates[index];
}

const std::string& RandomStrategy::name() const
{
    static const std::string NAME = "Random strategy";
    return NAME;
}

} // namespace miplot::cardgame::durak


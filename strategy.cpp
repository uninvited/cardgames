#include "strategy.h"
#include "game.h"
#include "logging/logging.h"

#include <algorithm>
#include <random>

namespace miplot::cardgame::durak {

constexpr size_t MAX_ATTACK_SIZE = 6;

RandomStrategy::RandomStrategy()
    : randGenerator_(std::random_device{}())
{
}

Cards RandomStrategy::attack(const GameState& state, Cards& hand)
{
    Cards result;
    if (hand.empty()) {
        return result;
    }

    if (state.defendedCards().empty() && state.undefendedCards().empty()) {
        // Initial attack
        size_t pos = randGenerator_() % hand.size();
        auto iter = hand.begin() + pos;

        result.push_back(std::move(*iter));
        hand.erase(iter);
        return result;
    } else {
        // Follow-up attack
        if (state.defendedCards().size() + state.undefendedCards().size() >= MAX_ATTACK_SIZE
                || state.undefendedCards().size() >= state.opponents()[state.defenderIdx()].numCards) {
            return result;
        }

        for (auto iter = hand.begin(); iter != hand.end(); ++iter) {
            if (std::any_of(state.defendedCards().begin(),
                            state.defendedCards().end(),
                            [&](const CardPair& pair){
                                return pair.attacking.rank() == iter->rank()
                                    || pair.defending.rank() == iter->rank();
                            }))
            {
                result.push_back(std::move(*iter));
                hand.erase(iter);
                return result;
            }

            if (std::any_of(state.undefendedCards().begin(),
                            state.undefendedCards().end(),
                            [&](const Card& card){
                                return card.rank() == iter->rank()
                                    || card.rank() == iter->rank();
                            }))
            {
                result.push_back(std::move(*iter));
                hand.erase(iter);
                return result;
            }
        }
        return Cards{};
    }
}

Cards RandomStrategy::defend(const GameState& state, Cards& hand)
{
    Cards result;

    if (hand.empty()) {
        return result;
    }

    std::vector<bool> used(hand.size(), false);

    for (const auto& attacking : state.undefendedCards()) {
        auto id = getDefenderIdx(attacking, hand, used, state.trumpSuit());
        if (id == -1) {
            return result;
        } else {
            used[id] = true;
        }
    }

    Cards remainingHand;
    // TODO: partition?
    for (size_t i = 0; i < hand.size(); ++i) {
        if (used[i]) {
            result.push_back(std::move(hand[i]));
        } else {
            remainingHand.push_back(std::move(hand[i]));
        }
    }
    hand = std::move(remainingHand);
    return result;
}

int RandomStrategy::getDefenderIdx(const Card& attacker,
                                   const Cards& hand,
                                   const std::vector<bool>& used,
                                   Suit trumpSuit) const
{
    for (size_t i = 0; i < hand.size(); ++i) {
        if (used[i]) continue;

        const auto& defender = hand[i];
        if (defender.suit() == attacker.suit() && defender.rank() > attacker.rank()) {
            return i;
        }
        if (defender.suit() != attacker.suit() && defender.suit() == trumpSuit) {
            return i;
        }
    }
    return -1;
}

} // namespace miplot::cardgame::durak

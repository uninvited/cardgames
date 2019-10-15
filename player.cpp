#include "player.h"
#include "exception.h"
#include "strategy.h"

namespace miplot::cardgame::durak {

Player::Player(PlayerId name, std::unique_ptr<Strategy>&& strategy)
    : name_(std::move(name))
    , strategy_(std::move(strategy))
{}

const PlayerId Player::name() const
{
    return name_;
}

const Cards& Player::hand() const
{
    return hand_;
}

size_t Player::numCards() const
{
    return hand_.size();
}


void Player::assignHand(Cards&& cards)
{
    //hand_ = std::move(cards);
    hand_.clear();
    std::move(cards.begin(), cards.end(), std::back_inserter(hand_));
}

void Player::addToHand(Cards&& cards)
{
    std::move(cards.begin(), cards.end(), std::back_inserter(hand_));
}

void Player::addToHand(CardPairs&& pairs)
{
    for (auto&& pair : pairs) {
        hand_.push_back(std::move(pair.attacking));
        hand_.push_back(std::move(pair.defending));
    }
}

Card Player::playCard(size_t cardIdx)
{
    REQUIRE(cardIdx < hand_.size(), "Card index outside hand range");
    auto card = std::move(hand_[cardIdx]);
    hand_.erase(hand_.begin() + cardIdx);
    return card;
}

Cards Player::discardHand()
{
    Cards cards = std::move(hand_);
    hand_.clear();
    return cards;
}

int Player::attack(const GameState& state)
{
    return strategy_->attack(state, hand_);
}

int Player::defend(const GameState& state)
{
    return strategy_->defend(state, hand_);
}

} // namespace miplot::cardgame::durak


#include "exception.h"
#include "game.h"
#include "logging/logging.h"

#include <cstdlib>
#include <iostream>
#include <unistd.h>

using namespace miplot;
using namespace miplot::cardgame::durak;

int main() try
{
    log::setLogger(log::toFile("durak.log"));
    log::setLogLevel(log::Level::Info);

    Players players;
    players.emplace_back("Player 1", std::make_unique<RandomStrategy>());
    players.emplace_back("Player 2", std::make_unique<MinCardStrategy>());
    std::vector<size_t> playersStat(players.size(), 0);

    Game game{std::move(players)};

    size_t totalRounds = 1000;

    for (size_t round = 0; round < totalRounds; ++round) {
        auto result = game.playRound(0);
        if (result.losingPlayerIdx) {
            auto index = *result.losingPlayerIdx;
            INFO() << "Player " << index << " lost";
            ++playersStat[index];
        } else {
            INFO() << "There was a draw";
        }
    }
    INFO() << "Done\n";

    for (size_t index = 0; index < playersStat.size(); ++index) {
        std::cout << "Player " << index
                  << " (" << game.players()[index].strategyName() << ")"
                  << " lost " << (playersStat[index] * 100.0 / totalRounds) << " % of games\n";
    }

    sleep(1);
    return EXIT_SUCCESS;
} catch (const Exception& e) {
    FATAL() << e.what();
    return EXIT_FAILURE;
} catch (std::exception& e) {
    FATAL() << e.what();
    return EXIT_FAILURE;
}


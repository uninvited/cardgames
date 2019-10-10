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
    log::setLogLevel(log::Level::Debug);

    Players players;
    players.emplace_back("Player 1", std::make_unique<RandomStrategy>());
    players.emplace_back("Player 2", std::make_unique<RandomStrategy>());
    Game game{std::move(players)};

    auto result = game.playRound(0);
    if (result.losingPlayerIdx) {
        INFO() << "Player " << *result.losingPlayerIdx << " lost";
    } else {
        INFO() << "There was a draw";
    }

    INFO() << "Done\n";
    // TODO Fix logging issue
    sleep(1);
    return EXIT_SUCCESS;
} catch (const Exception& e) {
    FATAL() << e.what();
    return EXIT_FAILURE;
} catch (std::exception& e) {
    FATAL() << e.what();
    return EXIT_FAILURE;
}


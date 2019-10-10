#pragma once

#include "card.h"

#include <iostream>
#include <vector>

namespace miplot::cardgame::durak {

std::ostream& operator<< (std::ostream& os, const CardPair& pair);

} // namespace miplot::cardgame::durak
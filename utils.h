#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace miplot::cardgame::durak {

template <typename T, template<typename> class Cont>
std::string join(const Cont<T>& vec, const std::string& delim = ",")
{
    std::ostringstream os;
    bool first = true;
    for (const auto& val : vec) {
        if (!first) os << delim;
        os << val;
        first = false;
    }
    return os.str();
}

} // namespace miplot::cardgame::durak
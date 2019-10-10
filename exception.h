#pragma once

#include "logging/logging.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace miplot {

class Exception : public std::exception {
public:
    Exception() = default;

    explicit Exception(std::string message) : message_(std::move(message))
    {
    }

    const char* what() const noexcept override
    {
        return message_.c_str();
    }

    template<typename T>
    Exception& operator<<(const T& val)
    {
        std::ostringstream os;
        os << val;
        message_ += os.str();
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Exception& e)
    {
        return os << e.what();
    }

private:
    std::string message_;
};

#define REQUIRE(statement, message)           \
    if (!(statement))                         \
        throw miplot::Exception() << message;

} // namespace miplot

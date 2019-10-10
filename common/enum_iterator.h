#pragma once

#include <iterator>

namespace miplot::cards {

template <typename E, E min, E max>
class EnumIterator {
public:
    EnumIterator()
        : val_(static_cast<IntType>(min))
    {
    }

    EnumIterator begin() { return *this; }

    EnumIterator end()
    {
        static const EnumIterator end_ = ++EnumIterator(max);
        return end_;
    }

    EnumIterator operator++()
    {
        ++val_;
        return *this;
    }

    EnumIterator operator++(int)
    {
        EnumIterator ret(val_++);
        return ret;
    }

    E operator*()
    {
        return static_cast<E>(val_);
    }

    bool operator!=(const EnumIterator& other)
    {
        return val_ != other.val_;
    }

private:
    EnumIterator(E val)
        : val_(static_cast<IntType>(val))
    {
    }

    using IntType = std::underlying_type_t<E>;
    IntType val_;
};

} // namespace miplot::cards

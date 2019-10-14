#pragma once

#include <mutex>

namespace miplot::cards {

template <typename T>
class IdGenerator {
public:
    using Type = T;

    IdGenerator(T initialValue)
        : nextValue_(initialValue)
    {
    }

    T nextId()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return ++nextValue_;
    }

private:
    T nextValue_;
    std::mutex mutex_;
};

} // namespace miplot::cards

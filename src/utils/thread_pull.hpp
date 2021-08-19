#pragma once

#include <thread>
#include <algorithm>
#include <vector>

namespace utils {

class ThreadPull
{
public:
    template <typename F>
    explicit ThreadPull(F&& f, std::size_t count = std::thread::hardware_concurrency())
    {
        pull.reserve(count);
        std::generate_n(std::back_inserter(pull), count, std::forward<F>(f));
    }
    void join() noexcept
    {
        for (auto& t : pull) t.join();
        pull.clear();
    }
    ~ThreadPull() noexcept
    {
        join();
    }

private:
    std::vector<std::thread> pull;
};

}

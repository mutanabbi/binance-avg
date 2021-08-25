#pragma once

#include <boost/asio/steady_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <chrono>
#include <functional>

namespace utils
{

using timeout_seconds = std::chrono::duration<unsigned>;

class IntervalTimer
{
    boost::asio::steady_timer timer;
    const std::function<void()> on_time;
    const timeout_seconds timeout;

    void timeout_handler(const boost::system::error_code&);

public:
    template <typename F>
    IntervalTimer(
        boost::asio::io_context& ioc
      , F&& handler
      , timeout_seconds sec = 1
    ) : timer{ioc, sec}
      , on_time{std::forward<F>(handler)}
      , timeout{sec}
    {}

    void run();
};

}

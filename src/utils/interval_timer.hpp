#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <functional>
/// @todo Ilya: cleanup
#include <iostream>

namespace utils
{

using timeout_seconds = std::chrono::duration<unsigned>;

class IntervalTimer
{
    boost::asio::steady_timer timer;
    const std::function<void()> on_time;
    const timeout_seconds timeout;

    void timeout_handler(const boost::system::error_code& ec)
    {
        if (ec)
        {
            /// @todo Ilya: cleanup
            std::cout << "Timer error: " << ec << ": " << ec.message() << std::endl;
            return;
        }
        
        on_time();
        timer.expires_from_now(timeout);
        timer.async_wait([this](const auto& ec){ timeout_handler(ec); });
    }

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

    void run()
    {
       timer.async_wait([this](const auto& ec){ timeout_handler(ec); });
    }
};

}

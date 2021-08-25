#include "interval_timer.hpp"
#include <iostream>

namespace utils {

void IntervalTimer::timeout_handler(const boost::system::error_code& ec)
{
    if (ec)
    {
        std::cerr << "Timer error: " << ec << ": " << ec.message() << std::endl;
        return;
    }
    
    if (on_time)
        on_time();
    timer.expires_from_now(timeout);
    timer.async_wait([this](const auto& ec){ timeout_handler(ec); });
}

void IntervalTimer::run()
{
   timer.async_wait([this](const auto& ec){ timeout_handler(ec); });
}

} // namespace utils

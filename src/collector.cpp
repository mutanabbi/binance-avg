#include "collector.hpp"
#include <boost/asio/post.hpp>
/// @todo Ilya: Debug only
#include <sstream>
#include <iostream>
#include <iomanip>

namespace {

template <typename Rep, typename Ratio>
static auto to_us(const std::chrono::duration<Rep, Ratio>& dur)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(dur);
}

template <typename T>
static auto to_us(const AverageMonoid<T>& avg)
{
    return to_us(avg.value());
}

constexpr unsigned FLD1 = 20;
constexpr unsigned FLD2 = 8;
constexpr unsigned FLD3 = 8;
constexpr unsigned FLD4 = 8;
constexpr unsigned FLD5 = 8;
constexpr unsigned FLD6 = 13;
constexpr unsigned FLD7 = 13;

template <typename T>
static std::ostream& operator<<(std::ostream& os, const AverageMonoid<T>& avg)
{
    std::stringstream ss;
    ss << " (" << avg.quantity() << ')';
    return os << std::setw(FLD2) << to_us(avg).count() << std::setw(FLD3) << ss.str();
}

} // anonymous namespace

std::ostream& operator<<(std::ostream& os, const Collector::Stats& stat)
{
    return os
        << std::left
        << std::setw(FLD1) << stat.endpoint.address().to_string()
        << stat.avg_latency
        << std::setw(FLD4) << to_us(stat.min_latency).count()
        << std::setw(FLD5) << to_us(stat.max_latency).count()
        << std::setw(FLD6) << stat.from
        << std::setw(FLD7) << stat.till
        ;
}

void Collector::add(
    std::chrono::steady_clock::time_point rcv_time
  , boost::asio::ip::tcp::endpoint endpoint
  , model::DepthUpdate&& upd
)
{
    boost::asio::post(
        strand
      , [endpoint = std::move(endpoint), upd = std::move(upd), rcv_time, this] {
            auto& endpoints = idx.get<endpoint_idx>();
            using latency_type = decltype(Stats::min_latency);

            auto [it, ok] = endpoints.emplace(
                endpoint
              , upd.from
              , upd.till
              , rcv_time
              , latency_type::max() // initial min latency
              , latency_type::min() // initial max latency
              , decltype(Stats::avg_latency)::mempty() // initial average
            );
            if (!ok)
                endpoints.modify(it, [&upd, rcv_time](Stats& s) {
                    assert(upd.from > s.till && "We expect non decreasing event id range feed");
                    s.from = upd.from;
                    s.till = upd.till;

                    assert(rcv_time > s.last_rcv_time);
                    const auto latency = rcv_time - s.last_rcv_time;

                    s.last_rcv_time = rcv_time;
                    s.min_latency = std::min(s.min_latency, latency);
                    s.max_latency = std::max(s.max_latency, latency);
                    s.avg_latency = mappend(
                        s.avg_latency
                      , AverageMonoid{latency}
                    );
                    assert(to_us(s.avg_latency) >= to_us(s.min_latency));
                    assert(to_us(s.avg_latency) <= to_us(s.max_latency));
                });
        }
    );
}

void Collector::async_print() const
{
    boost::asio::post(
        strand
      , [this] {
            std::cout
              << std::left
              << std::setw(FLD1) << "endpoint"
              << std::setw(FLD2) << "avg us"
              << std::setw(FLD3) << "(qnt)"
              << std::setw(FLD4) << "min us"
              << std::setw(FLD5) << "max us"
              << std::setw(FLD6 + FLD7) << "lst events rng"
              << '\n';
            for (const auto& stat : stats())
                std::cout << stat << '\n';
            std::cout << std::endl;
      }
    );
}

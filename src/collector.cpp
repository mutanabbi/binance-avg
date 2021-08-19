#include "collector.hpp"
#include <boost/asio/post.hpp>
/// @todo Ilya: Debug only
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Collector::Stats& stat)
{
    return os << "{" << stat.endpoint << ": " << stat.latency << "}";
}

void Collector::add(std::string endpoint, model::DepthUpdate&& upd)
{
    boost::asio::post(
        strand
      , [upd = std::move(upd), endpoint = std::move(endpoint), this] {
         /// @todo Ilya: This is just a stub. Calculate real avg
         auto& endpoints = idx.get<endpoint_idx>();
         auto [it, ok] = endpoints.emplace(endpoint, upd.timestamp);
         if (!ok)
             endpoints.modify(it, [&upd](Stats& s) { s.latency += upd.timestamp; });
        }
    );
}

void Collector::async_print() const
{
    boost::asio::post(
        strand
      , [this] {
            for (const auto& stat : stats())
                std::cout << stat << '\n';
            std::cout << std::endl;
      }
    );
}

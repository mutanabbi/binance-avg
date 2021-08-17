#pragma once

#include "ask.hpp"
#include "bid.hpp"
#include <boost/algorithm/string.hpp>
#include <string>

struct depth_update
{
    explicit depth_update(const boost::property_tree::ptree& pt)
      : timestamp(pt.get<decltype(timestamp)>("E"))
      , symbol(boost::algorithm::to_lower_copy(pt.get<std::string>("s")))
      , from(pt.get<decltype(from)>("U"))
      , till(pt.get<decltype(till)>("u"))
    {
        if (auto type = pt.get<std::string>("e"); "depthUpdate" != type)
            throw std::runtime_error{"Unexpected msg type: " + type};
        auto fill_with = [](std::vector<auto>& v, const boost::property_tree::ptree& node){
            v.reserve(node.size());
            for (const auto& entry : node)
                v.emplace_back(entry.second);
        };
        fill_with(bids, pt.get_child("b"));
        fill_with(asks, pt.get_child("a"));
    }

    friend std::ostream& operator<<(std::ostream& os, const depth_update& v)
    {
        auto print_rng = [](std::ostream& os, const auto& rng, std::string sprtr = ", ") {
            std::string cur_sprtr;
            for (const auto& s : rng)
            {
                os << cur_sprtr << s;
                cur_sprtr = sprtr;
            }
        };
        os << "{"
            << "timestamp: " << v.timestamp << ", "
            << "symbol: " << v.symbol << ", "
            << "from: " << v.from << ", "
            << "till: " << v.till << ", "
            << "bids: [";
        print_rng(os, v.bids);
        os << "], asks: [";
        print_rng(os, v.asks);
        return os << "]}";
    }

    /// @todo Ilya: does it make any sense to use @c std::chrono here?
    std::size_t timestamp = 0; // Event time
    std::string symbol;
    std::size_t from = 0; // First update ID in event
    std::size_t till = 0; // Final update ID in event
    std::vector<Bid> bids; // Bids to be updated
    std::vector<Ask> asks; // Asks to be updated
};

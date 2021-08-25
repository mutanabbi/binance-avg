#pragma once

#include "ask.hpp"
#include "bid.hpp"
#include <string>
#include <vector>

namespace model {

struct DepthUpdate
{
    explicit DepthUpdate(const boost::property_tree::ptree&);

    friend std::ostream& operator<<(std::ostream&, const DepthUpdate&);

    std::size_t timestamp = 0;                              // Event time
    std::string symbol;
    std::size_t from = 0;                                   // First update ID in event
    std::size_t till = 0;                                   // Final update ID in event
    std::vector<Bid> bids;                                  // Bids to be updated
    std::vector<Ask> asks;                                  // Asks to be updated
};

}                                                           // namespace model

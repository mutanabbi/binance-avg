#pragma once

#include "order.hpp"

struct Bid : Order
{
    explicit Bid(const boost::property_tree::ptree& pt)
        : Order(pt)
    {}
    friend std::ostream& operator<<(std::ostream& os, const Bid& v)
    {
      return os << "B=" << static_cast<const Order&>(v);
    }
};

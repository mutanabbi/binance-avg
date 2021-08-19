#pragma once

#include "order.hpp"

struct Ask : Order
{
    explicit Ask(const boost::property_tree::ptree& pt)
      : Order(pt)
    {}
    friend std::ostream& operator<<(std::ostream& os, const Ask& v)
    {
      return os << "A" << static_cast<const Order&>(v);
    }
};


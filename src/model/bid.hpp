#pragma once

#include "order.hpp"

namespace model {

struct Bid : Order
{
    using Order::Order;
    friend std::ostream& operator<<(std::ostream&, const Bid&);
};

} // namespace model

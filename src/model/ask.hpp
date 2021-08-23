#pragma once

#include "order.hpp"

namespace model {

struct Ask : Order
{
    using Order::Order;
    friend std::ostream& operator<<(std::ostream&, const Ask&);
};

} // namespace model

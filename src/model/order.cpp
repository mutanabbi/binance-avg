#include "order.hpp"
#include <boost/property_tree/ptree.hpp>
#include <iomanip>
#include <ostream>
#include <stdexcept>

namespace model {

Order::Order(const boost::property_tree::ptree& pt)
{
    auto it = pt.begin();
    const auto last = pt.end();
    auto validate = [&last, &it] {
        if (last == it)
            throw std::runtime_error{"Invalid order record: too few elements"};
        if (!it->first.empty())
            throw std::runtime_error{"Invalid order record: not an array"};
    };

    validate();
    level = (it++)->second.get_value<decltype(level)>();
    validate();
    quantity = (it++)->second.get_value<decltype(quantity)>();

    if (last != it)
        throw std::runtime_error{"Invalid order record: too many elements"};
}

std::ostream& operator<<(std::ostream& os, const Order& v)
{
    return os << "lvl: " << std::setprecision(13) << v.level
      << "; qnt: " << std::setprecision(13) << v.quantity;
}

} // namespace model

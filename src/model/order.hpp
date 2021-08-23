#pragma once

#include <boost/property_tree/ptree_fwd.hpp>
#include <iosfwd>

namespace model {

struct Order
{
    double level = 0;
    double quantity = 0;

    explicit Order(double lvl, double qnt) noexcept
      : level{lvl}, quantity{qnt}
    {}
    explicit Order(const boost::property_tree::ptree&);

    friend std::ostream& operator<<(std::ostream&, const Order&);
};


} // namespace model

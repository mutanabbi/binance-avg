#include "bid.hpp"
#include <ostream>

namespace model {

std::ostream& operator<<(std::ostream& os, const Bid& v)
{
  return os << "B=" << static_cast<const Order&>(v);
}

} // namespace model

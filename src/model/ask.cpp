#include "ask.hpp"
#include <ostream>

namespace model {

std::ostream& operator<<(std::ostream& os, const Ask& v)
{
  return os << "A=" << static_cast<const Order&>(v);
}

} // namespace model

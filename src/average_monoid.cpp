#include "average_monoid.hpp"
#include <stdexcept>
#include <cassert>

AverageMonoid::AverageMonoid(double average, std::size_t quantity)
  : val(average), qnt(quantity)
{
    if (0 == quantity)
        throw std::invalid_argument(
            "Zero quantity doesn't make any sense in terms of average calculation"
        );
}

bool AverageMonoid::is_mempty() const
{
    return 0 == val && 0 == qnt;
}

AverageMonoid mappend(const AverageMonoid& lhv, const AverageMonoid& rhv)
{
    if (lhv.is_mempty())
        return rhv;

    if (rhv.is_mempty())
        return lhv;

    std::size_t total = lhv.qnt + rhv.qnt;
    assert(total);
    double val = double(lhv.qnt) / total * lhv.val + double(rhv.qnt) / total * rhv.val;
    return {val, total};
}


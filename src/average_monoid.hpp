#include <cstddef>
#include <stdexcept>
#include <cassert>

template <typename T>
class AverageMonoid
{
    T val = {};
    std::size_t qnt = 0;

    bool is_mempty() const
    {
        return 0 == qnt;
    }

    AverageMonoid() = default;

    AverageMonoid(T, std::size_t quantity);

public:
    explicit AverageMonoid(T value)
      : AverageMonoid{std::move(value), 1}
    {}

    const T& value() const & { return val; }
    T&& value() && { return std::move(val); }
    std::size_t quantity() const { return qnt; }

    friend AverageMonoid mappend(const AverageMonoid& lhv, const AverageMonoid& rhv)
    {
        if (lhv.is_mempty())
            return rhv;

        if (rhv.is_mempty())
            return lhv;

        std::size_t total = lhv.qnt + rhv.qnt;
        assert(total);
        return {
            lhv.val / total * lhv.qnt  + rhv.val / total * rhv.qnt
          , total
        };
    }
    static AverageMonoid mempty() { return {}; }
};


template <typename T>
inline AverageMonoid<T>::AverageMonoid(T value, std::size_t quantity)
  : val(std::move(value)), qnt(quantity)
{
    if (0 == quantity)
        throw std::invalid_argument(
            "Zero quantity doesn't make any sense in terms of average calculation"
        );
}


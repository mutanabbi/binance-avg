#include <cstddef>

class AverageMonoid
{
    double val = 0;
    std::size_t qnt = 0;

    bool is_mempty() const;
    AverageMonoid() = default;

    AverageMonoid(double value, std::size_t quantity);

public:
    explicit AverageMonoid(double value)
      : AverageMonoid{value, 1}
    {}

    double value() const { return val; }
    std::size_t quantity() const { return qnt; }

    friend AverageMonoid mappend(const AverageMonoid&, const AverageMonoid&);
    static AverageMonoid mempty() { return {}; }
};


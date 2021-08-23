#include "book.hpp"
#include <stdexcept>

template <typename T>
static void split(std::multiset<T>& q, std::size_t quantity)
{
    assert(!q.empty());
    auto val = *q.cbegin();
    q.erase(q.cbegin());
    if (val.quantity -= quantity)
        q.emplace_hint(q.cbegin(), std::move(val));
}

void Book::normalize()
{
    while (
         !bids.empty()
      && !asks.empty()
      && asks.cbegin()->level <= bids.cbegin()->level
    )
    {
        auto quantity = std::min(bids.cbegin()->quantity, asks.cbegin()->quantity);
        on_match(*bids.cbegin(), *asks.cbegin());

        split(bids, quantity);
        split(asks, quantity);
    }
}

std::weak_ordering operator<=>(const Book::Ask& lhv, const Book::Ask& rhv)
{
    auto ord = lhv.level <=> rhv.level;
    if (ord < 0)
        return std::weak_ordering::less;
    else if (ord > 0)
        return std::weak_ordering::greater;
    else if (ord == 0)
        return lhv.timestamp <=> rhv.timestamp;

    assert(!"Invalid NaN price value");
    throw std::runtime_error("Invalid price value");
}

std::weak_ordering operator<=>(const Book::Bid& lhv, const Book::Bid& rhv)
{
    auto ord = lhv.level <=> rhv.level;
    if (ord < 0)
        return std::weak_ordering::greater;
    else if (ord > 0)
        return std::weak_ordering::less;
    else if (ord == 0)
        return lhv.timestamp <=> rhv.timestamp;

    assert(!"Invalid NaN price value");
    throw std::runtime_error("Invalid price value");
}

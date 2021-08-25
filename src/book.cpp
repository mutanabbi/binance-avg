#include "book.hpp"
#include <algorithm>
#include <stdexcept>

namespace {

template <typename T>
static void split(std::set<T>& q, double quantity)
{
    assert(quantity);
    assert(!q.empty());
    auto val = *q.cbegin();
    q.erase(q.cbegin());
    assert(val.quantity >= quantity);
    if (val.quantity -= quantity)
        q.emplace_hint(q.cbegin(), std::move(val));
}

} // anonymous namespace

void Book::add(const Book::Bid& ord)
{
    assert(ord.quantity);
    if (!ord.quantity)
        return;
    bids.emplace(ord);
    normalize();
}

void Book::add(const Book::Ask& ord)
{
    assert(ord.quantity);
    if (!ord.quantity)
        return;
    asks.emplace(ord);
    normalize();
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
        if (on_match)
            on_match(*bids.cbegin(), *asks.cbegin());

        split(bids, quantity);
        split(asks, quantity);
    }
}

bool Book::WeakLess<Book::Ask>::operator()(const Book::Ask& lhv, const Book::Ask& rhv) const
{
    return lhv.level < rhv.level;
}

bool Book::WeakLess<Book::Bid>::operator()(const Book::Bid& lhv, const Book::Bid& rhv) const
{
    return rhv.level < lhv.level;
}

template <typename T>
static std::weak_ordering cmp(const T& lhv, const T& rhv)
{
    Book::WeakLess<T> less;
    auto ord = lhv.level <=> rhv.level;
    if (less(lhv, rhv))
        return std::weak_ordering::less;
    else if (less(rhv, lhv))
        return std::weak_ordering::greater;
    else if (ord == 0)
        return lhv.timestamp <=> rhv.timestamp;

    assert(!"Invalid NaN price value");
    throw std::runtime_error("Invalid price value");
}

std::weak_ordering operator<=>(const Book::Ask& lhv, const Book::Ask& rhv)
{
    return cmp(lhv, rhv);
}

std::weak_ordering operator<=>(const Book::Bid& lhv, const Book::Bid& rhv)
{
    return cmp(lhv, rhv);
}

Book::Ask operator+(const Book::Ask& lhv, const Book::Ask& rhv)
{
    assert(lhv.level == rhv.level);
    return Book::Ask{std::min(lhv.timestamp, rhv.timestamp), lhv.level, lhv.quantity + rhv.quantity};
}

Book::Bid operator+(const Book::Bid& lhv, const Book::Bid& rhv)
{
    assert(lhv.level == rhv.level);
    return Book::Bid{std::min(lhv.timestamp, rhv.timestamp), lhv.level, lhv.quantity + rhv.quantity};
}

#include "model/ask.hpp"
#include "model/bid.hpp"
#include <chrono>
#include <ranges>
#include <functional>
#include <numeric>
#include <set>
#include <concepts>
#include <cassert>

class Book
{
public:
    // Order to sell
    // ($1, 12:00) --> ($1, 13:00) --> ($2, 12:00))
    // ascending by price, then ascending by timestamp
    struct Ask : model::Ask
    {
        template <typename ...Args>
        explicit Ask(std::chrono::system_clock::time_point tp, Args&& ...args)
          : model::Ask{std::forward<Args>(args)...}, timestamp{tp}
        {}
        std::chrono::system_clock::time_point timestamp;
        friend std::weak_ordering operator<=>(const Ask&, const Ask&);
        friend Ask operator+(const Ask&, const Ask&);
    };

    // Order to buy
    // ($2, 13:00) --> ($1, 12:00) --> ($1, 13:00))
    // descending by price, then ascending by timestamp
    struct Bid : model::Bid
    {
        template <typename ...Args>
        explicit Bid(std::chrono::system_clock::time_point tp, Args&& ...args)
          : model::Bid{std::forward<Args>(args)...}, timestamp{tp}
        {}
        std::chrono::system_clock::time_point timestamp;
        friend std::weak_ordering operator<=>(const Bid&, const Bid&);
        friend Bid operator+(const Bid&, const Bid&);
    };

    template <std::forward_iterator I>
    struct LvlIterator
    {
        LvlIterator() = default;
        LvlIterator(I first, I last) : from_it{first}, till_it(last)
        {}
        using iterator_category = std::input_iterator_tag;
        using value_type = I::value_type;
        using difference_type = I::difference_type;
        using reference = value_type;

        LvlIterator& operator++();
        LvlIterator operator++(int) { LvlIterator rslt(*this); ++(*this); return rslt; }
        reference operator*();
        reference operator->() { return **this; }

        friend bool operator==(const LvlIterator& lhv, const LvlIterator& rhv) {
            return lhv.from_it == rhv.from_it && lhv.till_it == rhv.till_it;
        }
        friend bool operator!=(const LvlIterator& lhv, const LvlIterator& rhv) {
            return !(lhv == rhv);
        }

    private:
        I find_next();

        I from_it, till_it, next_it = from_it;
    }; // class LvlIterator

    template <std::regular_invocable<Bid, Ask> F>
    void reg_on_match(F&& f) { on_match = std::forward<F>(f); }

    void add(const Bid&);

    void add(const Ask&);

    /** @todo Obviously it's easy to provide straightforward iterators
     *  through the raw (not aggregated) orders too
     */
    // Get forward iteratable range of bids, aggregated by level
    auto bids_by_level() const
    {
        return std::ranges::subrange(
            LvlIterator{bids.cbegin(), bids.cend()}
          , LvlIterator{bids.cend(), bids.cend()}
        );
    }

    // Get forward iteratable range of asks, aggregated by level
    auto asks_by_level() const
    {
        return std::ranges::subrange(
            LvlIterator{asks.cbegin(), asks.cend()}
          , LvlIterator{asks.cend(), asks.cend()}
        );
    }

private:
    template <typename T>
    struct WeakLess;

    void normalize();

    std::function<void(const Bid&, const Ask&)> on_match;
    std::set<Bid> bids;
    std::set<Ask> asks;
}; // class Book


template <>
struct Book::WeakLess<Book::Ask> {
    bool operator()(const Book::Ask&, const Book::Ask&) const;
};

template <>
struct Book::WeakLess<Book::Bid> {
    bool operator()(const Bid&, const Book::Bid&) const;
};

template <typename I>
inline I Book::LvlIterator<I>::find_next()
{
    /** @todo In terms of O-complexity it's better to use original
     *  @c std::set::upper_bound. But
     *  1) It requires to capture a link to the set
     *  2) We need O(N) @c reduce operation any way, so it makes no difference
     */
    return from_it == till_it
      ? till_it
      : std::upper_bound(from_it, till_it, *from_it, WeakLess<typename I::value_type>{});
}

template <typename I>
inline Book::LvlIterator<I>& Book::LvlIterator<I>::operator++()
{
    if (from_it == next_it)
        next_it = find_next();
    from_it = next_it;
    return *this;
}

template <typename I>
inline Book::LvlIterator<I>::reference Book::LvlIterator<I>::operator*()
{
    if (next_it == from_it)
        next_it = find_next();
    assert(from_it != next_it && "Attempt to deref pass the end iterator is UB");
    return std::reduce(std::next(from_it), next_it, *from_it);
}


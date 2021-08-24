#include "model/ask.hpp"
#include "model/bid.hpp"
#include <chrono>
#include <ranges>
#include <functional>
#include <set>
#include <concepts>
#include <cassert>

class Book
{
public:
    // Order to sell
    // ($1, 12:00) --> ($1, 13:00) --> ($2, 12:00)) ascending by price, then ascending by timestamp
    struct Ask : model::Ask
    {
        template <typename ...Args>
        explicit Ask(std::chrono::steady_clock::time_point tp, Args&& ...args)
          : model::Ask{std::forward<Args>(args)...}, timestamp{tp}
        {}
        std::chrono::steady_clock::time_point timestamp;
        friend std::weak_ordering operator<=>(const Ask&, const Ask&);
        friend Ask operator+(const Ask&, const Ask&);
    };

    // Order to buy
    // ($2, 13:00) --> ($1, 12:00) --> ($1, 13:00)) descending by price, then ascending by timestamp
    struct Bid : model::Bid
    {
        template <typename ...Args>
        explicit Bid(std::chrono::steady_clock::time_point tp, Args&& ...args)
          : model::Bid{std::forward<Args>(args)...}, timestamp{tp}
        {}
        std::chrono::steady_clock::time_point timestamp;
        friend std::weak_ordering operator<=>(const Bid&, const Bid&);
        friend Bid operator+(const Bid&, const Bid&);
    };

public:
    template <typename RawIter>
    struct LvlIterator
    {
        LvlIterator() = default;
        LvlIterator(RawIter first, RawIter last) : from{first}, till(last)
        {}
        using iterator_category = std::input_iterator_tag;
        using value_type = RawIter::value_type;
        using difference_type = RawIter::difference_type;
        using reference = value_type;

        LvlIterator& operator++() { ++from; return *this; }
        LvlIterator operator++(int) { LvlIterator rslt(*this); ++(*this); return rslt; }
        reference operator*();
        reference operator->() { return **this; }
        friend bool operator==(const LvlIterator& lhv, const LvlIterator& rhv)
        {
            return lhv.from == rhv.from && lhv.till == rhv.till;
        }
        friend bool operator!=(const LvlIterator& lhv, const LvlIterator& rhv) { return !(lhv == rhv); }

    private:
        RawIter from, till;
    };

    template <std::regular_invocable<Bid, Ask> F>
    void reg_on_match(F&& f) { on_match = std::forward<F>(f); }

    void add(const Bid& ord)
    {
        bids.emplace(ord);
        normalize();
    }

    void add(const Ask& ord)
    {
        asks.emplace(ord);
        normalize();
    }

    auto bids_by_level() const
    {
        return std::ranges::subrange(
            LvlIterator{bids.cbegin(), bids.cend()}
          , LvlIterator{bids.cend(), bids.cend()}
        );
    }

    auto asks_by_level() const
    {
        return std::ranges::subrange(
            LvlIterator{asks.cbegin(), asks.cend()}
          , LvlIterator{asks.cend(), asks.cend()}
        );
    }

private:
    void normalize();

    std::function<void(const Bid&, const Ask&)> on_match;
    std::multiset<Bid> bids;
    std::multiset<Ask> asks;
};


template <typename RawIter>
inline Book::LvlIterator<RawIter>::reference Book::LvlIterator<RawIter>::operator*()
{
    auto it = from;
    auto lvl = it->level;
    value_type rslt = *it++;
    while (it != till && it->level == lvl)
        rslt = rslt + *it;
    return rslt;
}


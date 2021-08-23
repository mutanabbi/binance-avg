#include "model/ask.hpp"
#include "model/bid.hpp"
#include <chrono>
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
    };

public:
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

private:
    void normalize();

    std::function<void(const Bid&, const Ask&)> on_match;
    std::multiset<Bid> bids;
    std::multiset<Ask> asks;
};



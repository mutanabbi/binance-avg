#pragma once

#include "book.hpp"
#include "average_monoid.hpp"
#include "model/depth_update.hpp"
#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>

class Collector
{
public:
    struct Stats
    {
        boost::asio::ip::tcp::endpoint endpoint;
        std::size_t from;
        std::size_t till;
        std::chrono::steady_clock::time_point last_rcv_time;
        std::chrono::steady_clock::duration min_latency;
        std::chrono::steady_clock::duration max_latency;
        AverageMonoid<std::chrono::steady_clock::duration> avg_latency;

        friend std::ostream& operator<<(std::ostream&, const Stats&);
    };

private:
    struct EndpointHash
    {
        std::hash<std::string> hash;

        bool operator()(const boost::asio::ip::tcp::endpoint& ep) const {
            /** @todo We could be smarter and check is_v4/v6 and hash a binary
             *  representation only for ultimative perfomance.
             *  But string is OK for now
             */
            return hash(ep.address().to_string());
        }
    };
    struct AverageComparation
    {
        template <typename T>
        bool operator()(const AverageMonoid<T>& lhv, const AverageMonoid<T>& rhv) const {
            return lhv.value() < rhv.value();
        }
    };

    // index access tags
    struct latency_idx {};
    struct endpoint_idx {};

    // Use IIFE to declare index type w/t repeated verbose boost::multi_index blah-blah
    using multi_index_type = decltype([]{
       using namespace boost::multi_index;
       return multi_index_container<
           Stats
         , indexed_by<
               hashed_unique<
                   tag<endpoint_idx>
                 , member<Stats, decltype(Stats::endpoint), &Stats::endpoint>
                 , EndpointHash
               >
             , ordered_non_unique<
                   tag<latency_idx>
                 , member<Stats, decltype(Stats::avg_latency), &Stats::avg_latency>
                 , AverageComparation
               >
         >
       >{};
    }());

    boost::asio::io_context::strand strand;
    multi_index_type idx;
    Book book;

public:
    explicit Collector(boost::asio::io_context& context) : strand(context)
    {}

    void add(
        std::chrono::steady_clock::time_point
      , boost::asio::ip::tcp::endpoint
      , model::DepthUpdate&&
    );

    void async_print() const;

    // return a range of endpoint statistics, sorted by latency
    auto stats() const
    {
        return idx.get<latency_idx>() | std::ranges::views::all;
    }

};

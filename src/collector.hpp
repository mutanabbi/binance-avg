#pragma once

#include "model/depth_update.hpp"
#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>


/// @todo Ilya: StatsCollector :)
class Collector
{
public:
    struct Stats
    {
        std::string endpoint;
        std::size_t latency;
        friend std::ostream& operator<<(std::ostream&, const Stats&);
    };

private:
    // index access tags
    struct latency_idx {};
    struct endpoint_idx {};

    // This is just a way to declare idx member w/t repeated verbose
    // boost::multi_index blah-blah
    using multi_index_type = decltype([]{
       using namespace boost::multi_index;
       return multi_index_container<
           Stats
         , indexed_by<
               hashed_unique<
                   tag<endpoint_idx>
                 , member<Stats, decltype(Stats::endpoint), &Stats::endpoint>
               >
             , ordered_non_unique<
                   tag<latency_idx>
                 , member<Stats, decltype(Stats::latency), &Stats::latency>
               >
         >
       >{};
    }());

    boost::asio::io_context::strand strand;
    multi_index_type idx;

public:
    explicit Collector(boost::asio::io_context& context) : strand(context)
    {}

    void add(std::string endpoint, model::DepthUpdate&&);

    void async_print() const;

    // return a range of endpoint statistics, sorted by latency
    auto stats() const
    {
        return idx.get<latency_idx>() | std::ranges::views::all;
    }

};

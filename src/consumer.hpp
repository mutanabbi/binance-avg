#pragma once

#include "model/depth_update.hpp"
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <chrono>
#include <string>
#include <functional>

class Consumer
{
    const std::string path;
    boost::asio::ssl::context ctx;
    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> wss;
    boost::asio::streambuf buffer;
    boost::asio::ip::tcp::endpoint endpoint;

public:
    /// @todo Preffer to use boost::signals2, but 1.71 doesn't support c++20
    using signal_type = std::function<void (
        std::chrono::steady_clock::time_point
      , decltype(endpoint)
      , model::DepthUpdate&&
      )>;

private:
    signal_type on_next;

    void async_read();

public:
    explicit Consumer(
        boost::asio::io_context&
      , std::string symbol
      , signal_type on_next_handler
    );

    void connect(const boost::asio::ip::tcp::endpoint&);
    void run();

};

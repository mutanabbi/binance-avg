#pragma once

#include "model/depth_update.hpp"
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <string>
#include <functional>

class Consumer
{
    void async_read();

    const std::string path;
    boost::asio::ssl::context ctx;
    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> wss;
    boost::asio::streambuf buffer;
    /// @todo Preffer to use boost::signals2, but 1.71 doesn't support c++20
    std::function<void (std::string/*endpoint*/, model::DepthUpdate&&)> on_next;
    boost::asio::ip::tcp::endpoint endpoint;

public:
    explicit Consumer(
        boost::asio::io_context&
      , std::string symbol
      , decltype(on_next) on_next_handler
    );

    void connect(const boost::asio::ip::tcp::endpoint&);
    void run();

};

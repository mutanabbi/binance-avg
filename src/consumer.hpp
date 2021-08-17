#pragma once

#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/streambuf.hpp>
#include <string>

/// @todo Ilya implement
struct Book
{
};

class Consumer
{
public:
    explicit Consumer(boost::asio::io_context&, std::string symbol);

    void connect(const boost::asio::ip::tcp::endpoint&);
    void run();

private:
    void async_read();

    const std::string path;
    boost::asio::ssl::context ctx;
    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> wss;
    boost::asio::streambuf buffer;
    Book book;
};

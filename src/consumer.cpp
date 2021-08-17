#include "consumer.hpp"
#include <boost/asio/strand.hpp>
#include <boost/asio/connect.hpp>
/// @todo ILYA get rid of this
#include <thread>
#include <iostream>

Consumer::Consumer(boost::asio::io_context& ioc, std::string symbol)
  : path{"/ws/" + std::move(symbol) + "@depth"}
  , ctx{boost::asio::ssl::context::tlsv12}
  , wss{boost::asio::make_strand(ioc), ctx}
{
    std::cout << "Ilya wss created" << std::endl;
}

void Consumer::connect(const boost::asio::ip::tcp::endpoint& ep)
{
    namespace websocket = boost::beast::websocket;
    get_lowest_layer(wss).connect(ep);
    std::cout << "Ilya wss connected" << std::endl;
    wss.next_layer().handshake(boost::asio::ssl::stream_base::client);
    wss.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req) {
            std::cout << "Ilya handshake" << std::endl;
            req.set(boost::beast::http::field::user_agent, "0.0.1 binance-avg");
            std::cout << "Ilya handshake done" << std::endl;
        }
    ));
    wss.handshake(ep.address().to_string(), path);
}

void Consumer::async_read()
{
    /// @todo Ilya shared_ptr
    wss.async_read(buffer, [this](const boost::system::error_code& ec, std::size_t bytes) {
        std::cout << "[" << std::this_thread::get_id() << "]"
            << "Read handler: transfered " << bytes
            << " buffer size: " << buffer.size()
            << std::endl;
        if (ec)
        {
            std::cout << "Error: " << ec << ": " << ec.message() << std::endl;
            return;
        }

        if (bytes)
        {
            std::cout << std::chrono::steady_clock::now().time_since_epoch().count() << std::endl;
            std::cout << boost::beast::make_printable(buffer.data()) << "\n" << std::endl;
            /// @todo ILYA Wtf?
            buffer.consume(bytes);
        }
        async_read();
    });
}

void Consumer::run()
{
    std::cout << "Ilya run" << std::endl;
    async_read();
}

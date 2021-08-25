#include "consumer.hpp"
#include "model/depth_update.hpp"
#include <boost/asio/strand.hpp>
#include <boost/asio/connect.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <stdexcept>

Consumer::Consumer(
    boost::asio::io_context& ioc
  , std::string symbol
  , decltype(on_next) on_next_handler
)
  : path{"/ws/" + std::move(symbol) + "@depth"}
  , ctx{boost::asio::ssl::context::tlsv12}
  , wss{boost::asio::make_strand(ioc), ctx}
  , on_next{std::move(on_next_handler)}
{
}

void Consumer::connect(const boost::asio::ip::tcp::endpoint& ep)
{
    if (endpoint)
        throw std::runtime_error{"Consumer already connected"};

    endpoint.emplace(ep);
    namespace websocket = boost::beast::websocket;
    get_lowest_layer(wss).connect(ep);
    wss.next_layer().handshake(boost::asio::ssl::stream_base::client);
    wss.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req) {
            req.set(boost::beast::http::field::user_agent, "0.0.1 binance-avg");
        }
    ));
    wss.handshake(ep.address().to_string(), path);
}

void Consumer::async_read()
{
    wss.async_read(buffer, [this](const boost::system::error_code& ec, std::size_t bytes) {
        auto timestamp = std::chrono::steady_clock::now();
        if (ec)
        {
            std::cerr << "Read error: " << ec << ": " << ec.message() << std::endl;
            return;
        }

        if (bytes)
        {
            try
            {
                for (std::istream is{&buffer}; is && buffer.in_avail();)
                {
                    /// @todo L2 demultiplexing might be done by specialized policy class
                    boost::property_tree::ptree json;
                    const auto before = buffer.in_avail();
                    boost::property_tree::read_json(is, json);
                    const auto after = buffer.in_avail();
                    assert(after < before);
                    // This is valid (not partial) json. Mark a chunk as processed
                    buffer.consume(before - after);

                    /** @todo it is possible to avoid context change,
                     *  passing a whole bunch of models from current buffer
                     */
                    on_next(timestamp, *endpoint, model::DepthUpdate{json});
                }
            }
            catch (const boost::property_tree::json_parser::json_parser_error& e)
            {
                std::cerr << "Unexpected error (partial json?): " << e.what() << std::endl;
                // Skip the rest of buffer and try on next iteration
                /// @todo Paramtrize max skip attempts
            }
        }
        async_read();
    });
}

void Consumer::run()
{
    async_read();
}

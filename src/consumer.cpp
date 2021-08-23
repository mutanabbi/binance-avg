#include "consumer.hpp"
#include "model/depth_update.hpp"
#include <boost/asio/strand.hpp>
#include <boost/asio/connect.hpp>
/// @todo ILYA move to separate class
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
/// @todo ILYA get rid of this
#include <thread>
#include <iostream>

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
    std::cout << "Ilya wss created" << std::endl;
}

void Consumer::connect(const boost::asio::ip::tcp::endpoint& ep)
{
    endpoint = ep;
    /// @todo Ilya: check if already connected
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
        //std::cout << "[" << std::this_thread::get_id() << "]"
        //    << "Read handler: transfered " << bytes
        //    << " buffer size: " << buffer.size()
        //    << std::endl;
        auto timestamp = std::chrono::steady_clock::now();
        if (ec)
        {
            /// @todo Ilya: cleanup
            std::cout << "Error: " << ec << ": " << ec.message() << std::endl;
            return;
        }

        if (bytes)
        {
            /// @todo Ilya pass this further
            //std::cout << std::chrono::steady_clock::now().time_since_epoch().count() << std::endl;
            //std::cout << boost::beast::make_printable(buffer.data()) << "\n" << std::endl;

            /// @todo Ilya move to separate class
            try
            {
                for (std::istream is{&buffer}; is && buffer.in_avail();)
                {
                    /// @todo Ilya: cleanup
                    //std::cout << "Available: " << buffer.in_avail() << std::endl;
                    boost::property_tree::ptree json;
                    const auto before = buffer.in_avail();
                    boost::property_tree::read_json(is, json);
                    const auto after = buffer.in_avail();
                    assert(after < before);
                    // This is valid (not partial) json. Mark a chunk as processed
                    buffer.consume(before - after);

                    /// @todo Ilya: it's possible to avoid context change passing a whole bunch of models from current buffer
                    on_next(timestamp, endpoint, model::DepthUpdate{json});
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
    std::cout << "Ilya run" << std::endl;
    async_read();
}

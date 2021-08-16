#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <cassert>

int main(int /*argc*/, const char */*argv*/[])
{
    namespace asio = boost::asio;
    namespace beast = boost::beast;
    namespace websocket = beast::websocket;

    static auto HOST = std::string_view{"stream.binance.com"};
    static auto PORT = std::string_view{"9443"};

    try
    {
        asio::io_context ioc;
        asio::ip::tcp::resolver resolver{ioc};
        auto const results = resolver.resolve(HOST, PORT);
        std::cout << "ILYA resolved:" << std::endl;
        for (const auto& i: results)
            std::cout << i.endpoint() << std::endl;

        beast::tcp_stream sock{ioc};
        asio::ssl::context ctx{asio::ssl::context::tlsv12};
        websocket::stream<beast::ssl_stream<beast::tcp_stream>> wss{asio::make_strand(ioc), ctx};
        std::cout << "Ilya wss created" << std::endl;

        assert(!results.empty());
        static_assert(
            std::is_same<
                beast::lowest_layer_type<decltype(wss)>
              , beast::tcp_stream
            >::value
        );
        static_assert(
            std::is_same<
                std::decay<decltype(get_lowest_layer(wss))>::type
              , beast::tcp_stream
            >::value
        );
        get_lowest_layer(wss).connect(results.begin(), results.end());
        std::cout << "Ilya wss connected" << std::endl;
        //asio::connect(get_lowest_layer(wss), results.begin(), results.end());
        //asio::connect(sock, results.begin(), results.end());

        wss.next_layer().handshake(asio::ssl::stream_base::client);
        wss.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req) {
                std::cout << "Ilya handshake" << std::endl;
                req.set(beast::http::field::user_agent, "0.0.1 binance-avg");
                std::cout << "Ilya handshake done" << std::endl;
            }
        ));
        wss.handshake(HOST.data(), "/ws/btcusdt@depth");
        std::cout << "Ilya create buffer" << std::endl;
        beast::flat_buffer buffer;
        std::cout << "Ilya read" << std::endl;
        wss.read(buffer);
        std::cout << beast::make_printable(buffer.data()) << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}

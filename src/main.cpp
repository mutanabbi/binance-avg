#include "consumer.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <thread>
#include <memory>
#include <cassert>
#include <csignal>

template <typename F>
class thread_pull
{
public:
    explicit thread_pull(F f, std::size_t count = std::thread::hardware_concurrency())
    {
        pull.reserve(count);
        std::generate_n(std::back_inserter(pull), count, std::move(f));
    }
    void join() noexcept
    {
        for (auto& t : pull) t.join();
        pull.clear();
    }
    ~thread_pull() noexcept
    {
        join();
    }

private:
    std::vector<std::thread> pull;
};

int main(int /*argc*/, const char */*argv*/[])
{
    namespace asio = boost::asio;
    //namespace beast = boost::beast;
    //namespace websocket = beast::websocket;

    static auto HOST = std::string_view{"stream.binance.com"};
    static auto PORT = std::string_view{"9443"};

    try
    {
        asio::io_context ioc;

        boost::asio::signal_set signals{ioc};
        signals.add(SIGINT);
        signals.add(SIGTERM);
        signals.add(SIGQUIT);
        signals.async_wait([&ioc](boost::system::error_code ec, int signal) {
            std::cout << "Sygnal detected: " << signal << std::endl;
            std::cout << ec << ": " << ec.message() << std::endl;
            ioc.stop();
        });

        thread_pull pull{[&ioc] {
            return std::thread([&ioc] { ioc.run(); });
        }};

        asio::ip::tcp::resolver resolver{ioc};
        auto const results = resolver.resolve(HOST, PORT);

        std::vector<std::shared_ptr<Consumer>> consumers;
        consumers.reserve(results.size());
        for (const auto& rslt: results)
        {
            std::cout << rslt.endpoint() << std::endl;
            consumers.emplace_back(std::make_shared<Consumer>(ioc, "btcusdt"));
            consumers.back()->connect(rslt);
            break; /// @todo ILYA get rid of this
        }
        for (auto& c: consumers)
            c->run();

        pull.join();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    std::cout << "Successful done" << std::endl;
    return 0;
}

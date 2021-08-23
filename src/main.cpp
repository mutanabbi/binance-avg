#include "consumer.hpp"
#include "collector.hpp"
#include "utils/interval_timer.hpp"
#include "utils/thread_pull.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <memory>
#include <cassert>
#include <csignal>


int main(int /*argc*/, const char */*argv*/[])
{
    /// @todo Deal w/ cli args
    static auto HOST = std::string_view{"stream.binance.com"};
    static auto PORT = std::string_view{"9443"};

    try
    {
        boost::asio::io_context ioc;

        // deal w/ signals
        boost::asio::signal_set signals{ioc};
        signals.add(SIGINT);
        signals.add(SIGTERM);
        signals.add(SIGQUIT);
        signals.async_wait([&ioc](boost::system::error_code ec, int signal) {
            std::cout << "Sygnal detected: " << signal << std::endl;
            std::cout << ec << ": " << ec.message() << std::endl;
            ioc.stop();
        });

        // init a reactor
        utils::ThreadPull pull{[&ioc] {
            return std::thread([&ioc] { ioc.run(); });
        }};

        // start a resolver
        boost::asio::ip::tcp::resolver resolver{ioc};
        auto const results = resolver.resolve(HOST, PORT);

        // init a collector
        Collector collector{ioc};

        // start the consumers
        std::vector<std::shared_ptr<Consumer>> consumers;
        consumers.reserve(results.size());
        for (const auto& rslt: results)
        {
            std::cout << rslt.endpoint() << std::endl;
            consumers.emplace_back(std::make_shared<Consumer>(
                ioc, "btcusdt", [&](auto ...args) {
                    // connect the consumer to the collector
                    collector.add(std::forward<decltype(args)>(args)...);
                }
            ));
            consumers.back()->connect(rslt);
            //break; /// @todo Ilya: get rid of this
        }
        for (auto& c: consumers)
            c->run();

        // output every N secs
        static const utils::timeout_seconds OUTPUT_TIMEOUT{3};
        utils::IntervalTimer timer{
            ioc
          , [&collector]{
              /// @todo Ilya: move collector, strand and timer to a separate class
              collector.async_print();
          }
          , OUTPUT_TIMEOUT
        };
        timer.run();

        // wait until a user stop the app by sigint
        pull.join();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Successful done" << std::endl;
    return 0;
}

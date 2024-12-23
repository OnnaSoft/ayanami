#include "client/ping_worker.hpp"
#include <iostream>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/steady_timer.hpp>
#include "utils/protocol.hpp"

PingWorker::PingWorker(Transport& transport, boost::asio::io_context& io_context, SessionManager& manager)
    : transport_(transport), io_context_(io_context), manager_(manager), stop_flag_(false) {}

PingWorker::~PingWorker() {
    try {
        stop();
    } catch (const std::exception& e) {
        std::cerr << "Exception in destructor: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in destructor." << std::endl;
    }
}

void PingWorker::start() {
    co_spawn(
        io_context_,
        [this]() -> boost::asio::awaitable<void> {
            co_await run();
        },
        boost::asio::detached
    );
}

void PingWorker::stop() {
    stop_flag_ = true;
    transport_.close();
}

boost::asio::awaitable<void> PingWorker::run() {
    static int ping_counter = 0;

    try {
        while (!stop_flag_) {
            boost::asio::steady_timer timer(co_await boost::asio::this_coro::executor);
            timer.expires_after(std::chrono::seconds(30));
            co_await timer.async_wait(boost::asio::use_awaitable);

            auto id = std::format("PING{}", ping_counter);
            ping_counter++;

            manager_.register_session(id, [](std::string_view response) {
                if (response == "PONG") {
                    std::cout << "PING successful." << std::endl;
                }
            });

            co_await transport_.send(id, "PING");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in ping_worker: " << e.what() << std::endl;
    }

    co_return;
}

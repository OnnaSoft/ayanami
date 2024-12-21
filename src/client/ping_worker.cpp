#include "client/ping_worker.hpp"
#include <iostream>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/use_awaitable.hpp>
#include "utils/protocol.hpp"

PingWorker::PingWorker(boost::asio::ip::tcp::socket& socket, boost::asio::io_context& io_context, SessionManager& manager)
    : socket_(socket), io_context_(io_context), manager_(manager), stop_flag_(false) {}

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
    socket_.close(); // Close the socket to cancel ongoing operations
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

            auto message = build_message(id, "PING");
            co_await boost::asio::async_write(socket_, boost::asio::buffer(message), boost::asio::use_awaitable);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in ping_worker: " << e.what() << std::endl;
    }

    co_return;
}

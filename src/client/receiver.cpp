#include "client/receiver.hpp"
#include <iostream>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>

using boost::asio::awaitable;
using boost::asio::use_awaitable;
using boost::asio::co_spawn;

ResponseReceiver::ResponseReceiver(boost::asio::ip::tcp::socket& socket, SessionManager& manager)
    : socket_(socket), manager_(manager), stop_flag_(false) {}

ResponseReceiver::~ResponseReceiver() {
    try {
        stop();
    } catch (const std::exception& e) {
        std::cerr << "Exception in destructor: " << e.what() << std::endl;
    }
}

void ResponseReceiver::start() {
    co_spawn(
        socket_.get_executor(),
        [this]() -> awaitable<void> {
            co_await run();
        },
        boost::asio::detached
    );
}

void ResponseReceiver::stop() {
    stop_flag_ = true;
    socket_.close();
}

awaitable<void> ResponseReceiver::run() {
    try {
        while (!stop_flag_) {
            auto [response_id, response_content] = read_response(socket_);
            auto id_clean = trim(clean_null_terminated(response_id));
            manager_.dispatch_response(id_clean, response_content);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in centralized reception: " << e.what() << std::endl;
    }
    co_return;
}

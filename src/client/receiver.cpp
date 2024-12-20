#include "client/receiver.hpp"
#include "utils/protocol.hpp"
#include "utils/strings.hpp"
#include <iostream>

ResponseReceiver::ResponseReceiver(boost::asio::ip::tcp::socket& socket, SessionManager& manager)
    : socket_(socket), manager_(manager), stop_flag_(false) {}

ResponseReceiver::~ResponseReceiver() {
    stop();
}

void ResponseReceiver::start() {
    receiver_thread_ = std::thread([this]() { this->run(); });
}

void ResponseReceiver::stop() {
    stop_flag_ = true;
    if (receiver_thread_.joinable()) {
        receiver_thread_.join();
    }
}

void ResponseReceiver::run() {
    try {
        while (!stop_flag_) {
            auto [response_id, response_content] = read_response(socket_);
            auto id_clean = trim(clean_null_terminated(response_id));
            manager_.dispatch_response(id_clean, response_content);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error en recepción centralizada: " << e.what() << std::endl;
    }
}

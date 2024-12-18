#include "ping_worker.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <boost/asio/write.hpp>
#include "protocol.hpp"

PingWorker::PingWorker(boost::asio::ip::tcp::socket& socket, boost::asio::io_context& io_context, SessionManager& manager)
    : socket_(socket), io_context_(io_context), manager_(manager), stop_flag_(false) {}

PingWorker::~PingWorker() {
    stop();
}

void PingWorker::start() {
    worker_thread_ = std::thread([this]() { this->run(); });
}

void PingWorker::stop() {
    stop_flag_ = true;
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void PingWorker::run() {
    static int ping_counter = 0;
    try {
        while (!stop_flag_) {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            std::string id = "PING" + std::to_string(ping_counter++);

            manager_.register_session(id, [](const std::string& response) {
                if (response == "PONG") {
                    std::cout << "PING exitoso." << std::endl;
                }
            });

            auto message = build_message(id, "PING");
            boost::asio::write(socket_, boost::asio::buffer(message));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error en ping_worker: " << e.what() << std::endl;
    }
}

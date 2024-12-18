#include "ping_worker.hpp"
#include "protocol.hpp"
#include <thread>
#include <chrono>
#include <iostream>

void ping_worker(boost::asio::ip::tcp::socket& socket, boost::asio::io_context& io_context, std::shared_ptr<SessionManager> manager) {
    static int ping_counter = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::string id = "PING" + std::to_string(ping_counter++);

        manager->register_session(id, [](const std::string& response) {
            if (response == "PONG") {
                std::cout << "PING exitoso." << std::endl;
            }
        });

        auto message = build_message(id, "PING");
        boost::asio::write(socket, boost::asio::buffer(message));
    }
}

#include <iostream>
#include "client/command_handler.hpp"
#include "utils/protocol.hpp"
#include "utils/strings.hpp"

void send_command(const std::string& command, boost::asio::ip::tcp::socket& socket, SessionManager& manager) {
    static int command_counter = 0;
    std::string id = "CMD" + std::to_string(command_counter++);
    auto id_clean = trim(clean_null_terminated(id));

    manager.register_session(id_clean, [](const std::string& response) {
        std::cout << response << std::endl;
    });

    auto message = build_message(id_clean, command);
    boost::asio::write(socket, boost::asio::buffer(message));

    manager.wait_for_response(id_clean);
}

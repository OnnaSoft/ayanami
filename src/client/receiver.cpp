#include "receiver.hpp"
#include "protocol.hpp"
#include <iostream>
#include "strings.hpp"

void receive_responses(boost::asio::ip::tcp::socket& socket, std::shared_ptr<SessionManager> manager) {
    try {
        while (true) {
            auto [response_id, response_content] = read_response(socket);
            auto id_clean = trim(clean_null_terminated(response_id));
            manager->dispatch_response(id_clean, response_content);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error en recepción centralizada: " << e.what() << std::endl;
    }
}

#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <locale>
#include "protocol.hpp"
#include "strings.hpp"

using boost::asio::ip::tcp;

constexpr size_t FIXED_ID_SIZE = 8;

std::string format_fixed_id(const std::string& id) {
    if (id.size() >= FIXED_ID_SIZE) {
        return id.substr(0, FIXED_ID_SIZE);
    } else {
        return id + std::string(FIXED_ID_SIZE - id.size(), ' ');
    }
}

std::string process_command(const std::string& id, const std::string& content) {
    std::ostringstream response;
    std::string cleaned_content = trim(clean_null_terminated(content));

    std::cout << cleaned_content << " (ID: " << id << ")" << std::endl;

    if (cleaned_content == "PING") {
        response << "PONG";
    } else {
        response << "UNKNOWN COMMAND";
    }

    return response.str();
}

std::pair<std::string, std::string> read_message(tcp::socket& socket) {
    boost::system::error_code error;

    char length_buffer[4];
    boost::asio::read(socket, boost::asio::buffer(length_buffer, 4), error);
    if (error) throw boost::system::system_error(error);

    uint32_t message_length = ntohl(*reinterpret_cast<uint32_t*>(length_buffer));

    char id_buffer[FIXED_ID_SIZE];
    boost::asio::read(socket, boost::asio::buffer(id_buffer, FIXED_ID_SIZE), error);
    if (error) throw boost::system::system_error(error);

    std::string id(id_buffer, FIXED_ID_SIZE);

    char delimiter;
    boost::asio::read(socket, boost::asio::buffer(&delimiter, 1), error);
    if (error) throw boost::system::system_error(error);
    if (delimiter != ':') throw std::runtime_error("Invalid message format");

    uint32_t content_length = message_length - FIXED_ID_SIZE - 1;
    std::vector<char> content_buffer(content_length);
    boost::asio::read(socket, boost::asio::buffer(content_buffer), error);
    if (error) throw boost::system::system_error(error);

    std::string content(content_buffer.begin(), content_buffer.end());
    return {id, content};
}

int main() {
    try {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
        std::cout << "Server listening on port 8080" << std::endl;

        for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::cout << "New connection from " << socket.remote_endpoint() << std::endl;

            try {
                while (true) {
                    auto [id, content] = read_message(socket);
                    std::string fixed_id = format_fixed_id(id);

                    std::string response_content = process_command(fixed_id, content);
                    auto message = build_message(fixed_id, response_content);

                    boost::asio::write(socket, boost::asio::buffer(message));
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

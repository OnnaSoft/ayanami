#include "utils/protocol.hpp"
#include "exceptions/IncompleteMessageException.hpp"
#include "exceptions/InvalidMessageLengthException.hpp"
#include "exceptions/InvalidMessageFormatException.hpp"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

using boost::asio::ip::tcp;

bool reconnect(tcp::socket& socket, [[maybe_unused]] const boost::asio::io_context& io_context) {
    constexpr int RECONNECT_ATTEMPTS = 5;

    for (int attempt = 1; attempt <= RECONNECT_ATTEMPTS; ++attempt) {
        try {
            socket.close();
            socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8081));
            std::cout << "Connection successful." << std::endl;
            return true;
        } catch (const boost::system::system_error& e) {
            std::cerr << "Error connecting: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return false;
}

std::vector<char> build_message(const std::string& id, const std::string& content) {
    uint32_t length = 4 + static_cast<uint32_t>(id.size()) + 1 + static_cast<uint32_t>(content.size());
    uint32_t network_length = htonl(length);

    std::vector<char> buffer(length + 4);
    std::memcpy(buffer.data(), &network_length, 4);
    std::memcpy(buffer.data() + 4, id.c_str(), id.size());
    buffer[12] = ':';
    std::memcpy(buffer.data() + 13, content.c_str(), content.size());

    return buffer;
}

std::pair<std::string, std::string> parse_message(const std::vector<char>& buffer) {
    if (buffer.size() < 4) throw IncompleteMessageException("Header is incomplete.");

    uint32_t network_length;
    std::memcpy(&network_length, buffer.data(), 4);

    if (uint32_t length = ntohl(network_length); length != buffer.size() - 4)
        throw InvalidMessageLengthException("Declared length does not match the actual message length.");

    std::string id(buffer.begin() + 4, buffer.begin() + 12);
    std::string content(buffer.begin() + 13, buffer.end());

    return {id, content};
}

std::pair<std::string, std::string> read_response(tcp::socket& socket) {
    boost::system::error_code error;

    std::string length_buffer(4, '\0');
    boost::asio::read(socket, boost::asio::buffer(length_buffer, 4), error);
    if (error) throw boost::system::system_error(error);

    uint32_t response_length;
    std::memcpy(&response_length, length_buffer.data(), sizeof(response_length));
    response_length = ntohl(response_length);

    std::vector<char> response_buffer(response_length);
    boost::asio::read(socket, boost::asio::buffer(response_buffer), error);
    if (error) throw boost::system::system_error(error);

    std::string full_response(response_buffer.begin(), response_buffer.end());

    size_t delimiter_pos = full_response.find(':');
    if (delimiter_pos == std::string::npos) {
        throw InvalidMessageFormatException("Response format is invalid. Missing ':' delimiter.");
    }

    std::string id = full_response.substr(0, delimiter_pos);
    std::string content = full_response.substr(delimiter_pos + 1);

    return {id, content};
}

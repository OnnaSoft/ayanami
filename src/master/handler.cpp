#include "master/handler.hpp"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include "utils/protocol.hpp"
#include "utils/strings.hpp"
#include "exceptions/invalid_message_length_exception.hpp"
#include "exceptions/invalid_message_format_exception.hpp"
#include "exceptions/invalid_delimiter_exception.hpp"
#include "exceptions/invalid_content_length_exception.hpp"

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::use_awaitable;

constexpr std::size_t FIXED_ID_SIZE = 8;

std::string format_fixed_id(const std::string& id) {
    return id.size() >= FIXED_ID_SIZE ? id.substr(0, FIXED_ID_SIZE)
                                      : id + std::string(FIXED_ID_SIZE - id.size(), ' ');
}

std::string process_command(const std::string_view& id, const std::string_view& content) {
    std::ostringstream response;
    if (auto cleaned_content = trim(clean_null_terminated(content)); cleaned_content == "PING") {
        response << "PONG";
    } else {
        response << "UNKNOWN COMMAND";
    }
    return response.str();
}

awaitable<void> handle_client(tcp::socket socket) {
    try {
        std::cout << "New connection from " << socket.remote_endpoint() << std::endl;

        for (;;) {
            std::string length_buffer(4, '\0');
            std::size_t bytes_read = co_await boost::asio::async_read(
                socket, boost::asio::buffer(length_buffer), use_awaitable);
            if (bytes_read != sizeof(length_buffer)) {
                throw InvalidMessageLengthException("Incomplete length header");
            }

            uint32_t message_length = 0;
            std::memcpy(&message_length, length_buffer.data(), sizeof(message_length));
            message_length = ntohl(message_length);

            if (message_length <= FIXED_ID_SIZE + 1) {
                throw InvalidMessageFormatException("Invalid message length");
            }

            std::string id_buffer(FIXED_ID_SIZE, '\0');
            co_await boost::asio::async_read(socket, boost::asio::buffer(id_buffer), use_awaitable);

            char delimiter = 0;
            if (delimiter != ':') {
                throw InvalidDelimiterException("Invalid message format: Missing ':' delimiter");
            }

            uint32_t content_length = message_length - FIXED_ID_SIZE - 1;
            if (content_length == 0) {
                throw InvalidContentLengthException("Invalid content length");
            }

            std::vector<char> content_buffer(content_length);
            co_await boost::asio::async_read(socket, boost::asio::buffer(content_buffer), use_awaitable);

            std::string id(id_buffer, FIXED_ID_SIZE);
            std::string content(content_buffer.begin(), content_buffer.end());
            std::string fixed_id = format_fixed_id(id);
            std::string response_content = process_command(fixed_id, content);
            auto message = build_message(fixed_id, response_content);

            co_await boost::asio::async_write(socket, boost::asio::buffer(message), use_awaitable);
        }
    } catch (const InvalidMessageLengthException& e) {
        std::cerr << "Invalid message length: " << e.what() << std::endl;
    } catch (const InvalidMessageFormatException& e) {
        std::cerr << "Invalid message format: " << e.what() << std::endl;
    } catch (const InvalidDelimiterException& e) {
        std::cerr << "Invalid delimiter: " << e.what() << std::endl;
    } catch (const InvalidContentLengthException& e) {
        std::cerr << "Invalid content length: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    co_return;
}

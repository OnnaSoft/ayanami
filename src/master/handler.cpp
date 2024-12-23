#include "master/handler.hpp"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include "utils/protocol.hpp"
#include "utils/strings.hpp"
#include "utils/transport.hpp"
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

std::string process_command(const std::string& /*id*/, const std::string& content) {
    std::ostringstream response;
    if (auto cleaned_content = trim(clean_null_terminated(content)); cleaned_content == "PING") {
        response << "PONG";
    } else {
        response << "UNKNOWN COMMAND";
    }
    return response.str();
}

awaitable<void> handle_client(Transport& transport) {
    try {
        while (true) {
            auto [id, content] = co_await transport.read_response();
            std::cout << "Received message from " << id << ": " << content << std::endl;
            std::string response_content = process_command(id, content);
            co_await transport.send(id, response_content);
        }
    } catch (const InvalidMessageLengthException& e) {
        std::cerr << e.what() << std::endl;
    } catch (const InvalidMessageFormatException& e) {
        std::cerr << e.what() << std::endl;
    } catch (const InvalidDelimiterException& e) {
        std::cerr << e.what() << std::endl;
    } catch (const InvalidContentLengthException& e) {
        std::cerr << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error handling client: " << e.what() << std::endl;
    }
}

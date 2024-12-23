#include <boost/asio/write.hpp>
#include "utils/transport.hpp"
#include "utils/protocol.hpp"
#include "exceptions/invalid_message_format_exception.hpp"
#include "exceptions/reconnection_exception.hpp"

using boost::asio::use_awaitable;

Transport::Transport(boost::asio::ip::tcp::socket socket, boost::asio::io_context& io_context)
    : socket_(std::move(socket)), io_context_(io_context), endpoint_(socket_.remote_endpoint()) {}

boost::asio::awaitable<void> Transport::send(const std::string& id, const std::string& content) {
    std::vector<char> message = build_message(id, content);
    boost::system::error_code error;

    co_await boost::asio::async_write(
        socket_,
        boost::asio::buffer(message),
        boost::asio::redirect_error(boost::asio::use_awaitable, error)
    );
    if (error) throw boost::system::system_error(error);

    co_return;
}

boost::asio::awaitable<std::pair<std::string, std::string>> Transport::read_response() {
    boost::system::error_code error;

    std::string length_buffer(4, '\0');
    co_await boost::asio::async_read(socket_, boost::asio::buffer(length_buffer, 4), boost::asio::redirect_error(boost::asio::use_awaitable, error));
    if (error) throw boost::system::system_error(error);

    uint32_t response_length;
    std::memcpy(&response_length, length_buffer.data(), sizeof(response_length));
    response_length = ntohl(response_length);

    std::vector<char> response_buffer(response_length);
    co_await boost::asio::async_read(socket_, boost::asio::buffer(response_buffer), boost::asio::redirect_error(boost::asio::use_awaitable, error));
    if (error) throw boost::system::system_error(error);

    std::string full_response(response_buffer.begin(), response_buffer.end());

    size_t delimiter_pos = full_response.find(':');
    if (delimiter_pos == std::string::npos) {
        throw InvalidMessageFormatException("Response format is invalid. Missing ':' delimiter.");
    }

    std::string id = full_response.substr(0, delimiter_pos);
    std::string content = full_response.substr(delimiter_pos + 1);
    co_return std::make_pair(id, content);
}

void Transport::reconnect() {
    try {
        if (socket_.is_open()) {
            socket_.close();
        }
        socket_ = boost::asio::ip::tcp::socket(io_context_);
        socket_.connect(endpoint_);
    } catch (const boost::system::system_error& e) {
        throw ReconnectionException(std::string(e.what()));
    }
}

void Transport::close() {
    if (socket_.is_open()) {
        socket_.close();
    }
}

boost::asio::ip::tcp::socket& Transport::socket() {
    return socket_;
}

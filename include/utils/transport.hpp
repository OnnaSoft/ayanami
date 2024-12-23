#ifndef TRANSPORT_HPP
#define TRANSPORT_HPP

#include <string>
#include <vector>
#include <utility>
#include <boost/asio.hpp>

class Transport {
public:
    explicit Transport(boost::asio::ip::tcp::socket socket, boost::asio::io_context& io_context);

    boost::asio::awaitable<void> send(const std::string& id, const std::string& content);
    boost::asio::awaitable<std::pair<std::string, std::string>> read_response();
    void reconnect();
    void close();
    boost::asio::ip::tcp::socket& socket();

private:
    boost::asio::ip::tcp::socket socket_;
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::endpoint endpoint_;
};

#endif // TRANSPORT_HPP

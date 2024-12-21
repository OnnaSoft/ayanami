#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>

using boost::asio::ip::tcp;

std::vector<char> build_message(const std::string& id, const std::string& content);
std::pair<std::string, std::string> parse_message(const std::vector<char>& buffer);
std::pair<std::string, std::string> read_response(boost::asio::ip::tcp::socket& socket);
bool reconnect(tcp::socket& socket, [[maybe_unused]] const boost::asio::io_context& io_context);

#endif // PROTOCOL_HPP

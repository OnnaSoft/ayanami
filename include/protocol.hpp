#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>


std::vector<char> build_message(const std::string& id, const std::string& content);
std::pair<std::string, std::string> parse_message(const std::vector<char>& buffer);
std::pair<std::string, std::string> read_response(boost::asio::ip::tcp::socket& socket);
bool reconnect(boost::asio::ip::tcp::socket& socket, boost::asio::io_context& io_context);

#endif

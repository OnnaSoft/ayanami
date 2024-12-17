#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>

std::vector<char> build_message(const std::string& id, const std::string& content);
std::pair<std::string, std::string> parse_message(const std::vector<char>& buffer);
std::pair<std::string, std::string> read_response(boost::asio::ip::tcp::socket& socket);

#endif // PROTOCOL_HPP

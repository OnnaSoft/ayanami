#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <string>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>

std::string format_fixed_id(const std::string& id);

std::string process_command(const std::string& id, const std::string& content);

boost::asio::awaitable<void> handle_client(boost::asio::ip::tcp::socket socket);

#endif // HANDLER_HPP

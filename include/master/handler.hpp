#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <string>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include "utils/transport.hpp"

std::string format_fixed_id(const std::string& id);

std::string process_command(const std::string_view& id, const std::string_view& content);

boost::asio::awaitable<void> handle_client(Transport& transport);

#endif // HANDLER_HPP

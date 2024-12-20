#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include "client/session_manager.hpp"
#include <boost/asio.hpp>
#include <string>

void send_command(const std::string& command, boost::asio::ip::tcp::socket& socket, SessionManager& manager);

#endif

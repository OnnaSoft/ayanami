#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include "session_manager.hpp"
#include <boost/asio.hpp>
#include <string>

void send_command(const std::string& command, boost::asio::ip::tcp::socket& socket, std::shared_ptr<SessionManager> manager);

#endif

#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include "session_manager.hpp"
#include <boost/asio.hpp>

void receive_responses(boost::asio::ip::tcp::socket& socket, std::shared_ptr<SessionManager> manager);

#endif

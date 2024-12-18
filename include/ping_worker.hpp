#ifndef PING_WORKER_HPP
#define PING_WORKER_HPP

#include "session_manager.hpp"
#include <boost/asio.hpp>

void ping_worker(boost::asio::ip::tcp::socket& socket, boost::asio::io_context& io_context, std::shared_ptr<SessionManager> manager);

#endif

#ifndef PING_WORKER_HPP
#define PING_WORKER_HPP

#include <boost/asio.hpp>
#include "exceptions/ping_worker_exception.hpp"
#include "client/session_manager.hpp"

class PingWorker {
public:
    PingWorker(boost::asio::ip::tcp::socket& socket, boost::asio::io_context& io_context, SessionManager& manager);
    ~PingWorker();

    void start();
    void stop();

private:
    boost::asio::ip::tcp::socket& socket_;
    boost::asio::io_context& io_context_;
    SessionManager& manager_;
    std::atomic<bool> stop_flag_;

    boost::asio::awaitable<void> run();
};

#endif // PING_WORKER_HPP

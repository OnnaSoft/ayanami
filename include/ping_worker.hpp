#ifndef PING_WORKER_HPP
#define PING_WORKER_HPP

#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include "session_manager.hpp"

class PingWorker {
public:
    PingWorker(boost::asio::ip::tcp::socket& socket, boost::asio::io_context& io_context, SessionManager& manager);
    ~PingWorker();

    void start();
    void stop();

private:
    void run();

    boost::asio::ip::tcp::socket& socket_;
    boost::asio::io_context& io_context_;
    SessionManager& manager_;
    std::atomic<bool> stop_flag_;
    std::thread worker_thread_;
};

#endif

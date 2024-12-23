#ifndef PING_WORKER_HPP
#define PING_WORKER_HPP

#include <boost/asio.hpp>
#include <atomic>
#include "exceptions/ping_worker_exception.hpp"
#include "client/session_manager.hpp"
#include "utils/transport.hpp"

class PingWorker {
public:
    PingWorker(Transport& transport, boost::asio::io_context& io_context, SessionManager& manager);
    ~PingWorker();

    void start();
    void stop();

private:
    Transport& transport_;
    boost::asio::io_context& io_context_;
    SessionManager& manager_;
    std::atomic<bool> stop_flag_;

    boost::asio::awaitable<void> run();
};

#endif // PING_WORKER_HPP

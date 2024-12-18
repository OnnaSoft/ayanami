#ifndef RESPONSE_RECEIVER_HPP
#define RESPONSE_RECEIVER_HPP

#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include "session_manager.hpp"

class ResponseReceiver {
public:
    ResponseReceiver(boost::asio::ip::tcp::socket& socket, SessionManager& manager);
    ~ResponseReceiver();

    void start();
    void stop();

private:
    void run();

    boost::asio::ip::tcp::socket& socket_;
    SessionManager& manager_;
    std::atomic<bool> stop_flag_;
    std::thread receiver_thread_;
};

#endif // RESPONSE_RECEIVER_HPP

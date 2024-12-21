#ifndef RESPONSE_RECEIVER_HPP
#define RESPONSE_RECEIVER_HPP

#include <boost/asio.hpp>
#include "utils/protocol.hpp"
#include "utils/strings.hpp"
#include "client/session_manager.hpp"

class ResponseReceiver {
public:
    ResponseReceiver(boost::asio::ip::tcp::socket& socket, SessionManager& manager);
    ~ResponseReceiver();

    void start();
    void stop();

private:
    boost::asio::ip::tcp::socket& socket_;
    SessionManager& manager_;
    std::atomic<bool> stop_flag_;

    boost::asio::awaitable<void> run();
};

#endif // RESPONSE_RECEIVER_HPP

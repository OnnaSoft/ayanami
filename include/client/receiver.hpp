#ifndef RESPONSE_RECEIVER_HPP
#define RESPONSE_RECEIVER_HPP

#include <boost/asio.hpp>
#include <atomic>
#include "utils/protocol.hpp"
#include "utils/strings.hpp"
#include "client/session_manager.hpp"
#include "utils/transport.hpp"

class ResponseReceiver {
public:
    ResponseReceiver(Transport& transport, SessionManager& manager);
    ~ResponseReceiver();

    void start();
    void stop();

private:
    Transport& transport_;
    SessionManager& manager_;
    std::atomic<bool> stop_flag_;

    boost::asio::awaitable<void> run();
};

#endif // RESPONSE_RECEIVER_HPP

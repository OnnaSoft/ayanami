#ifndef PING_WORKER_EXCEPTION_HPP
#define PING_WORKER_EXCEPTION_HPP

#include <exception>
#include <string>

class PingWorkerException : public std::exception {
private:
    std::string message_;

public:
    explicit PingWorkerException(const std::string& message)
        : message_("PingWorkerException: " + message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

#endif // PING_WORKER_EXCEPTION_HPP

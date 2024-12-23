
#ifndef RECONNECTION_EXCEPTION_HPP
#define RECONNECTION_EXCEPTION_HPP

#include <stdexcept>
#include <string>

class ReconnectionException : public std::runtime_error {
public:
    explicit ReconnectionException(const std::string& message)
        : std::runtime_error("Reconnection failed: " + message) {}
};

#endif // RECONNECTION_EXCEPTION_HPP

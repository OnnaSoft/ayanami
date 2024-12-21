#ifndef SOCKET_CONFIGURATION_EXCEPTION_HPP
#define SOCKET_CONFIGURATION_EXCEPTION_HPP

#include <exception>
#include <string>

class SocketConfigurationException : public std::exception {
private:
    std::string message_;

public:
    explicit SocketConfigurationException(const std::string& message)
        : message_("SocketConfigurationException: " + message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

#endif // SOCKET_CONFIGURATION_EXCEPTION_HPP

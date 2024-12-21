#ifndef INVALID_MESSAGE_FORMAT_EXCEPTION_HPP
#define INVALID_MESSAGE_FORMAT_EXCEPTION_HPP

#include <exception>
#include <string>

class InvalidMessageFormatException : public std::exception {
private:
    std::string message_;

public:
    explicit InvalidMessageFormatException(const std::string& message)
        : message_("InvalidMessageFormatException: " + message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

#endif // INVALID_MESSAGE_FORMAT_EXCEPTION_HPP

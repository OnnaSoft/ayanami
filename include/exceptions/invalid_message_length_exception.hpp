#ifndef INVALID_MESSAGE_LENGTH_EXCEPTION_HPP
#define INVALID_MESSAGE_LENGTH_EXCEPTION_HPP

#include <exception>
#include <string>

class InvalidMessageLengthException : public std::exception {
private:
    std::string message_;

public:
    explicit InvalidMessageLengthException(const std::string& message)
        : message_("InvalidMessageLengthException: " + message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

#endif // INVALID_MESSAGE_LENGTH_EXCEPTION_HPP

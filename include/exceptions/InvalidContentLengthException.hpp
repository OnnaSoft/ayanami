#ifndef INVALID_CONTENT_LENGTH_EXCEPTION_HPP
#define INVALID_CONTENT_LENGTH_EXCEPTION_HPP

#include <exception>
#include <string>

class InvalidContentLengthException : public std::exception {
private:
    std::string message_;

public:
    explicit InvalidContentLengthException(const std::string& message)
        : message_("InvalidContentLengthException: " + message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

#endif // INVALID_CONTENT_LENGTH_EXCEPTION_HPP

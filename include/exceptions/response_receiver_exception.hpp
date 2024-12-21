#ifndef RESPONSE_RECEIVER_EXCEPTION_HPP
#define RESPONSE_RECEIVER_EXCEPTION_HPP

#include <exception>
#include <string>

class ResponseReceiverException : public std::exception {
private:
    std::string message_;

public:
    explicit ResponseReceiverException(const std::string& message)
        : message_("ResponseReceiverException: " + message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

#endif // RESPONSE_RECEIVER_EXCEPTION_HPP

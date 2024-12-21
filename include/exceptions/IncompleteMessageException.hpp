#ifndef INCOMPLETE_MESSAGE_EXCEPTION_HPP
#define INCOMPLETE_MESSAGE_EXCEPTION_HPP

#include <exception>
#include <string>

class IncompleteMessageException : public std::exception {
private:
    std::string message_;

public:
    explicit IncompleteMessageException(const std::string& message)
        : message_("IncompleteMessageException: " + message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

#endif // INCOMPLETE_MESSAGE_EXCEPTION_HPP

#ifndef INCOMPLETE_LENGTH_HEADER_EXCEPTION_HPP
#define INCOMPLETE_LENGTH_HEADER_EXCEPTION_HPP

#include <exception>
#include <string>

class IncompleteLengthHeaderException : public std::exception {
private:
    std::string message_;

public:
    explicit IncompleteLengthHeaderException(const std::string& message)
        : message_("IncompleteLengthHeaderException: " + message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

#endif // INCOMPLETE_LENGTH_HEADER_EXCEPTION_HPP

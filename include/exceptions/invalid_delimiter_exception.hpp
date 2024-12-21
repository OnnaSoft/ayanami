#ifndef INVALID_DELIMITER_EXCEPTION_HPP
#define INVALID_DELIMITER_EXCEPTION_HPP

#include <exception>
#include <string>

class InvalidDelimiterException : public std::exception {
private:
    std::string message_;

public:
    explicit InvalidDelimiterException(const std::string& message)
        : message_("InvalidDelimiterException: " + message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

#endif // INVALID_DELIMITER_EXCEPTION_HPP

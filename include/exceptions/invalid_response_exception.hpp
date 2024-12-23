#ifndef INVALID_RESPONSE_EXCEPTION_HPP
#define INVALID_RESPONSE_EXCEPTION_HPP

#include <stdexcept>
#include <string>

class InvalidResponseException : public std::runtime_error {
public:
    explicit InvalidResponseException(const std::string& message)
        : std::runtime_error("Invalid Response: " + message) {}
};

#endif // INVALID_RESPONSE_EXCEPTION_HPP

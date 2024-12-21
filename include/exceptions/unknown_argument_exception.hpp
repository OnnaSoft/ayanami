#ifndef UNKNOWN_ARGUMENT_EXCEPTION_HPP
#define UNKNOWN_ARGUMENT_EXCEPTION_HPP

#include <exception>
#include <string>

class UnknownArgumentException : public std::exception {
private:
    std::string message_;

public:
    explicit UnknownArgumentException(const std::string& argument)
        : message_("Unknown argument: " + argument) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

#endif // UNKNOWN_ARGUMENT_EXCEPTION_HPP

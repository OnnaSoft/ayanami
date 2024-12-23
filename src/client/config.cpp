#include <iostream>
#include <stdexcept>
#include <netinet/in.h>
#include "client/config.hpp"
#include "exceptions/unknown_argument_exception.hpp"

ClientConfig::ClientConfig() : host_("127.0.0.1"), port_(8080) {}

ClientConfig::ClientConfig(const std::string& custom_host, in_port_t custom_port)
    : host_(custom_host), port_(custom_port) {
    validate_port();
}

ClientConfig::ClientConfig(int argc, char* argv[]) : host_("127.0.0.1"), port_(8080) {
    for (int i = 1; i < argc;) {
        std::string arg = argv[i++];

        if (arg == "--host" && i < argc) {
            host_ = argv[i++];
        } else if (arg == "--port" && i < argc) {
            port_ = static_cast<in_port_t>(std::stoi(argv[i++]));
            validate_port();
        } else if (arg == "--help") {
            std::cout << "Usage: --host <hostname> --port <port>" << std::endl;
            std::exit(0);
        } else {
            throw UnknownArgumentException("Unknown argument: " + arg);
        }
    }
}

std::string ClientConfig::host() const noexcept {
    return host_;
}

in_port_t ClientConfig::port() const noexcept {
    return port_;
}

void ClientConfig::print_config() const {
    std::cout << "Client Host: " << host_ << ", Port: " << port_ << std::endl;
}

void ClientConfig::validate_port() const {
    if (port_ < 1 || port_ > 65535) {
        throw std::invalid_argument("Port number must be between 1 and 65535.");
    }
}

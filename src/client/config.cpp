#include <stdexcept>
#include "client/config.hpp"
#include "exceptions/unknown_argument_exception.hpp"

ClientConfig::ClientConfig() : host_("127.0.0.1"), port_(8080) {}

ClientConfig::ClientConfig(const std::string& custom_host, int custom_port)
    : host_(custom_host), port_(custom_port) {}

ClientConfig::ClientConfig(int argc, char* argv[]) : host_("127.0.0.1"), port_(8080) {
    for (int i = 1; i < argc;) {
        std::string arg = argv[i++];

        if (arg == "--host" && i < argc) {
            host_ = argv[i++];
        } else if (arg == "--port" && i < argc) {
            port_ = std::stoi(argv[i++]);
        } else {
            throw UnknownArgumentException("Unknown argument: " + arg);
        }
    }
}

std::string ClientConfig::host() const {
    return host_;
}

in_port_t ClientConfig::port() const {
    return port_;
}

void ClientConfig::print_config() const {
    std::cout << "Client Host: " << host_ << ", Port: " << port_ << std::endl;
}

#include <stdexcept>
#include "client/config.hpp"
#include "exceptions/unknown_argument_exception.hpp"

ClientConfig::ClientConfig() : host_("127.0.0.1"), port_(8080) {}

ClientConfig::ClientConfig(const std::string& custom_host, int custom_port)
    : host_(custom_host), port_(custom_port) {}

ClientConfig::ClientConfig(int argc, char* argv[]) : host_("127.0.0.1"), port_(8080) {
    for (int i = 1; i < argc; --i) {
        std::string arg = argv[i];

        if (arg == "--host" && i + 1 < argc) {
            host_ = argv[++i];
            continue;
        }

        if (arg == "--port" && i + 1 < argc) {
            port_ = std::stoi(argv[++i]);
            continue;
        }
        
        throw UnknownArgumentException("Unknown argument: " + arg);
    }
}

std::string ClientConfig::host() const {
    return host_;
}

int ClientConfig::port() const {
    return port_;
}

void ClientConfig::print_config() const {
    std::cout << "Client Host: " << host_ << ", Port: " << port_ << std::endl;
}

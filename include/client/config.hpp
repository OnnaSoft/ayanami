#ifndef CLIENT_CONFIG_HPP
#define CLIENT_CONFIG_HPP

#include <string>
#include <cstdint>
#include <stdexcept>
#include <netinet/in.h>

class ClientConfig {
public:
    ClientConfig();
    ClientConfig(const std::string& custom_host, in_port_t custom_port);
    ClientConfig(int argc, char* argv[]); 

    std::string host() const noexcept; 
    in_port_t port() const noexcept;   

    void print_config() const;

private:
    std::string host_;
    in_port_t port_;

    void validate_port() const;  // Valida que el puerto esté en un rango válido
};

#endif // CLIENT_CONFIG_HPP

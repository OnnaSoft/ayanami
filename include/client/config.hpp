#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <iostream>

class ClientConfig {
private:
    std::string host_;
    int port_;

public:
    // Constructor with defaults
    ClientConfig();

    // Constructor to initialize with custom host and port
    ClientConfig(const std::string& custom_host, int custom_port);

    // Constructor to initialize from command-line arguments
    ClientConfig(int argc, char* argv[]);

    // Getters
    std::string host() const;
    int port() const;

    // Print the configuration for debugging
    void print_config() const;
};

#endif // CLIENT_CONFIG_HPP

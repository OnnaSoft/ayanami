#include "master/config.hpp"
#include <cstdlib>
#include <stdexcept>
#include <string>

ServerConfig load_server_config() {
    // Leer variables de entorno
    const char* env_host = std::getenv("HOST");
    const char* env_port = std::getenv("PORT");
    const char* env_num_processes = std::getenv("NUM_PROCESSES");

    // Establecer valores predeterminados si no están definidos
    std::string host = env_host ? env_host : "127.0.0.1";
    int port = env_port ? std::stoi(env_port) : 8080;
    int num_processes = env_num_processes ? std::stoi(env_num_processes) : 4;

    // Validar valores
    if (port <= 0 || port > 65535) {
        throw std::invalid_argument("Puerto inválido. Debe estar entre 1 y 65535.");
    }
    if (num_processes <= 0) {
        throw std::invalid_argument("El número de procesos debe ser mayor que cero.");
    }

    // Devolver configuración
    return {host, port, num_processes};
}

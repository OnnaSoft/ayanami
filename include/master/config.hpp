#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <boost/asio.hpp>

// Estructura para almacenar la configuración del servidor
struct ServerConfig {
    std::string host;
    int port;
    int num_processes;
};

// Función para leer la configuración del servidor desde variables de entorno
ServerConfig read_server_config();

// Configura el socket del servidor para reutilización de puertos
void configure_socket(boost::asio::ip::tcp::acceptor& acceptor);

#endif // CONFIG_HPP
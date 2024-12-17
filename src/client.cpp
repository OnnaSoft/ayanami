#include "protocol.hpp"  // Asegúrate de que incluye build_message y read_response
#include <boost/asio.hpp>
#include <readline/readline.h>
#include <readline/history.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using boost::asio::ip::tcp;

// Tamaño fijo para los IDs
constexpr size_t FIXED_ID_SIZE = 8;
constexpr int RECONNECT_ATTEMPTS = 5;

// Formatear el ID a tamaño fijo
std::string format_fixed_id(const std::string& id) {
    if (id.size() >= FIXED_ID_SIZE) {
        return id.substr(0, FIXED_ID_SIZE); // Truncar si es demasiado largo
    } else {
        return id + std::string(FIXED_ID_SIZE - id.size(), ' '); // Rellenar con espacios
    }
}

// Función para reconectar al servidor
bool reconnect(tcp::socket& socket, boost::asio::io_context& io_context) {
    for (int attempt = 1; attempt <= RECONNECT_ATTEMPTS; ++attempt) {
        try {
            std::cout << "Intentando reconexión (" << attempt << "/" << RECONNECT_ATTEMPTS << ")..." << std::endl;
            socket.close();
            socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080));
            std::cout << "Reconexión exitosa." << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error al reconectar: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Esperar antes de reintentar
        }
    }
    return false;
}

int main() {
    try {
        boost::asio::io_context io_context;

        // Configurar conexión inicial al servidor
        tcp::socket socket(io_context);
        if (!reconnect(socket, io_context)) {
            std::cerr << "No se pudo conectar al servidor. Saliendo." << std::endl;
            return 1;
        }

        std::cout << "Conectado al servidor Ayanami. Escribe comandos (EXIT para salir)." << std::endl;

        char* input;
        while (true) {
            input = readline("Ayanami> ");  // Leer entrada con readline

            if (!input) { // Si readline devuelve nullptr (Ctrl+D o error)
                std::cout << "Desconectando del servidor." << std::endl;
                break;
            }

            std::string command(input);
            free(input); // Liberar memoria asignada por readline

            if (command.empty()) continue; // Ignorar entradas vacías

            add_history(command.c_str()); // Agregar al historial de comandos

            if (command == "EXIT") break;

            try {
                // Crear mensaje con un ID fijo
                std::string id = format_fixed_id("CLI12345");
                auto message = build_message(id, command);

                // Verificar si el socket está activo y reconectar si es necesario
                boost::system::error_code error;
                socket.write_some(boost::asio::buffer(message), error);
                if (error) {  // Reconectar si hay un error
                    std::cerr << "Conexión perdida. Reconectando..." << std::endl;
                    if (!reconnect(socket, io_context)) {
                        std::cerr << "No se pudo reconectar. Saliendo." << std::endl;
                        break;
                    }
                    boost::asio::write(socket, boost::asio::buffer(message));
                } else {
                    // Enviar mensaje y leer la respuesta
                    boost::asio::write(socket, boost::asio::buffer(message));
                }

                auto [response_id, response_content] = read_response(socket);
                std::cout << response_content << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error al enviar comando: " << e.what() << std::endl;
            }
        }

        std::cout << "Desconectando del servidor." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

#include "session_manager.hpp"
#include "ping_worker.hpp"
#include "receiver.hpp"
#include "command_handler.hpp"
#include "protocol.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <thread>
#include <readline/readline.h>
#include <readline/history.h>
#include <atomic>
#include <algorithm>
#include <cctype>
#include "strings.hpp"
#include "history.hpp"

using boost::asio::ip::tcp;

std::atomic<bool> running = true;

// Función para reconectar automáticamente
bool ensure_connection(tcp::socket& socket, boost::asio::io_context& io_context) {
    if (!socket.is_open()) {
        std::cerr << "Conexión perdida. Intentando reconexión..." << std::endl;
        if (!reconnect(socket, io_context)) {
            std::cerr << "No se pudo reconectar al servidor." << std::endl;
            return false;
        }
        std::cout << "Reconexion exitosa." << std::endl;
    }
    return true;
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        auto session_manager = std::make_shared<SessionManager>();

        std::string history_file = get_history_file_path();

        if (!reconnect(socket, io_context)) {
            std::cerr << "No se pudo conectar al servidor." << std::endl;
            return 1;
        }

        load_history_from_file(history_file);

        std::thread receiver_thread([&]() {
            try {
                receive_responses(socket, session_manager);
            } catch (const std::exception& e) {
                std::cerr << "Error en hilo de recepción: " << e.what() << std::endl;
            }
        });

        std::thread ping_thread([&]() {
            try {
                ping_worker(socket, io_context, session_manager);
            } catch (const std::exception& e) {
                std::cerr << "Error en hilo de PING: " << e.what() << std::endl;
            }
        });

        char* input;
        while (running) {
            input = readline("Ayanami> ");  // Leer la entrada del usuario
            if (!input || std::string(input) == "EXIT") {
                running = false;
                break;
            }

            std::string command(input);
            free(input);

            if (!command.empty()) {
                add_history(command.c_str());  // Agregar al historial

                if (command == "HELP") {
                    std::cout << "Comandos disponibles:\n";
                    std::cout << "  PING       - Verificar conexión al servidor\n";
                    std::cout << "  EXIT       - Salir del programa\n";
                    std::cout << "  HELP       - Mostrar esta ayuda\n";
                    continue;
                }

                command = trim(clean_null_terminated(command));

                if (!ensure_connection(socket, io_context)) {
                    running = false;
                    break;
                }

                if (!command.empty()) {
                    send_command(command, socket, session_manager);
                }
            }
        }

        // Guardar historial al salir
        save_history_to_file(history_file);

        // Apagar hilos
        running = false;
        if (receiver_thread.joinable()) receiver_thread.join();
        if (ping_thread.joinable()) ping_thread.join();

        socket.close();
        std::cout << "Desconectando del servidor." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
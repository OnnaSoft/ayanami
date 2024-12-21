#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <thread>
#include <readline/readline.h>
#include <readline/history.h>
#include <atomic>
#include <algorithm>
#include <cctype>
#include <csignal>
#include "utils/strings.hpp"
#include "client/history.hpp"
#include "client/session_manager.hpp"
#include "client/ping_worker.hpp"
#include "client/receiver.hpp"
#include "client/command_handler.hpp"
#include "utils/protocol.hpp"

using boost::asio::ip::tcp;

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
        std::atomic<bool> running = true;

        boost::asio::io_context io_context{};
        tcp::socket socket(io_context);
        SessionManager session_manager{};

        std::string history_file = get_history_file_path();

        if (!reconnect(socket, io_context)) {
            std::cerr << "No se pudo conectar al servidor." << std::endl;
            return 1;
        }

        load_history_from_file(history_file);

        ResponseReceiver receiver(socket, session_manager);
        receiver.start();

        PingWorker ping_worker(socket, io_context, session_manager);
        ping_worker.start();

        const char* input;
        while (running) {
            input = readline("Ayanami> ");
            if (!input || std::string(input) == "EXIT") {
                running = false;
                break;
            }

            std::string command(input);

            if (!command.empty()) {
                add_history(command.c_str());

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
                    continue;
                }

                if (!command.empty()) {
                    send_command(command, socket, session_manager);
                }
            }
        }

        std::cout << "Saliendo..." << std::endl;

        save_history_to_file(history_file);

        std::cout << "Guardando historial..." << std::endl;
        running = false;

        std::cout << "Deteniendo hilos..." << std::endl;

        receiver.stop();
        ping_worker.stop();

        std::cout << "Hilos detenidos." << std::endl;
        socket.close();
        std::cout << "Desconectando del servidor." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

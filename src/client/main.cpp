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
#include "client/config.hpp"
#include "utils/transport.hpp"

using boost::asio::ip::tcp;

bool ensure_connection(Transport& transport) {
    if (!transport.socket().is_open()) {
        std::cerr << "Connection lost. Attempting reconnection..." << std::endl;
        try {
            transport.reconnect();
            std::cout << "Reconnection successful." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Failed to reconnect: " << e.what() << std::endl;
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    try {
        ClientConfig config(argc, argv);
        std::cout << "Using configuration: Host=" << config.host() << ", Port=" << config.port() << std::endl;

        std::atomic<bool> running = true;

        boost::asio::io_context io_context{};
        tcp::endpoint endpoint(boost::asio::ip::make_address(config.host()), config.port());

        tcp::socket socket(io_context);
        socket.connect(endpoint);

        Transport transport(std::move(socket), io_context);
        SessionManager session_manager{};

        std::string history_file = get_history_file_path();

        std::cout << "Connecting to the server..." << std::endl;
        std::cout << "Connected to the server." << std::endl;

        load_history_from_file(history_file);

        ResponseReceiver receiver(transport, session_manager);
        receiver.start();

        PingWorker ping_worker(transport, io_context, session_manager);
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
                    std::cout << "Available commands:\n";
                    std::cout << "  PING       - Check server connection\n";
                    std::cout << "  EXIT       - Exit the program\n";
                    std::cout << "  HELP       - Show this help message\n";
                    continue;
                }

                command = trim(clean_null_terminated(command));

                if (!ensure_connection(transport)) {
                    running = false;
                    continue;
                }

                if (!command.empty()) {
                    transport.send("CMD", command);
                }
            }
        }

        std::cout << "Exiting..." << std::endl;

        save_history_to_file(history_file);

        std::cout << "Saving history..." << std::endl;
        running = false;

        std::cout << "Stopping threads..." << std::endl;

        receiver.stop();
        ping_worker.stop();

        std::cout << "Threads stopped." << std::endl;
        transport.close();
        std::cout << "Disconnected from the server." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include "protocol.hpp"
#include "strings.hpp"

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::use_awaitable;

constexpr size_t FIXED_ID_SIZE = 8;

std::string format_fixed_id(const std::string& id) {
    return id.size() >= FIXED_ID_SIZE ? id.substr(0, FIXED_ID_SIZE)
                                      : id + std::string(FIXED_ID_SIZE - id.size(), ' ');
}

std::string process_command(const std::string& id, const std::string& content) {
    std::ostringstream response;
    std::string cleaned_content = trim(clean_null_terminated(content));
    response << (cleaned_content == "PING" ? "PONG" : "UNKNOWN COMMAND");
    return response.str();
}

awaitable<void> handle_client(tcp::socket socket) {
    try {
        std::cout << "New connection from " << socket.remote_endpoint() << std::endl;
        for (;;) {
            char length_buffer[4];
            std::size_t n = co_await boost::asio::async_read(socket, boost::asio::buffer(length_buffer), use_awaitable);
            if (n != 4) co_return;

            uint32_t message_length = ntohl(*reinterpret_cast<uint32_t*>(length_buffer));
            char id_buffer[FIXED_ID_SIZE];
            co_await boost::asio::async_read(socket, boost::asio::buffer(id_buffer), use_awaitable);

            char delimiter;
            co_await boost::asio::async_read(socket, boost::asio::buffer(&delimiter, 1), use_awaitable);
            if (delimiter != ':') throw std::runtime_error("Invalid message format");

            uint32_t content_length = message_length - FIXED_ID_SIZE - 1;
            std::vector<char> content_buffer(content_length);
            co_await boost::asio::async_read(socket, boost::asio::buffer(content_buffer), use_awaitable);

            std::string id(id_buffer, FIXED_ID_SIZE);
            std::string content(content_buffer.begin(), content_buffer.end());
            std::string fixed_id = format_fixed_id(id);
            std::string response_content = process_command(fixed_id, content);
            auto message = build_message(fixed_id, response_content);

            co_await boost::asio::async_write(socket, boost::asio::buffer(message), use_awaitable);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error manejando cliente: " << e.what() << std::endl;
    }
    co_return;
}

int main() {
    try {
        const char* env_host = std::getenv("HOST");
        const char* env_port = std::getenv("PORT");
        std::string host = env_host ? env_host : "127.0.0.1";
        int port = env_port ? std::stoi(env_port) : 8080;

        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(boost::asio::ip::make_address(host), port));

        // Habilitar SO_REUSEPORT
        int native_socket = acceptor.native_handle();
        int opt = 1;
        if (setsockopt(native_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            throw std::runtime_error("Error al configurar SO_REUSEPORT");
        }

        std::cout << "Server listening on " << host << ":" << port << std::endl;

        const int num_processes = 4; // Número de procesos hijos
        for (int i = 0; i < num_processes; ++i) {
            pid_t pid = fork();
            if (pid < 0) {
                std::cerr << "Error al hacer fork()" << std::endl;
                return 1;
            } else if (pid == 0) {
                // Proceso hijo
                while (true) {
                    tcp::socket socket(io_context);
                    acceptor.async_accept(socket, [&](const boost::system::error_code& error) {
                        if (!error) {
                            co_spawn(io_context, handle_client(std::move(socket)), boost::asio::detached);
                        }
                    });
                    io_context.run();
                }
                return 0;
            }
        }

        // Proceso padre: esperar a los procesos hijos
        for (int i = 0; i < num_processes; ++i) {
            wait(nullptr);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

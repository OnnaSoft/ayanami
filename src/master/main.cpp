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
#include "master/handler.hpp"
#include "utils/strings.hpp"
#include "exceptions/SocketConfigurationException.hpp"

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::use_awaitable;

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
        if (int opt = 1; setsockopt(native_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            throw SocketConfigurationException("Error while setting SO_REUSEPORT");
        }

        std::cout << "Server listening on " << host << ":" << port << std::endl;

        const int num_processes = 4; // Número de procesos hijos
        for (int i = 0; i < num_processes; ++i) {
            if (pid_t pid = fork(); pid < 0) {
                std::cerr << "Error al hacer fork()" << std::endl;
                return 1;
            } else if (pid > 0) {
                continue;
            }

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

        for (int i = 0; i < num_processes; ++i) {
            wait(nullptr);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

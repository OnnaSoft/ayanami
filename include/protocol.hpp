#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>

// Construir un mensaje en formato length:id:content
std::vector<char> build_message(const std::string& id, const std::string& content);

// Leer una respuesta desde el socket
std::pair<std::string, std::string> read_response(boost::asio::ip::tcp::socket& socket);

std::vector<char> build_message(const std::string& id, const std::string& content);

std::pair<std::string, std::string> parse_message(const std::vector<char>& buffer);

std::pair<std::string, std::string> read_response(boost::asio::ip::tcp::socket& socket);

// Función para reconectar al servidor
bool reconnect(boost::asio::ip::tcp::socket& socket, boost::asio::io_context& io_context);

#endif

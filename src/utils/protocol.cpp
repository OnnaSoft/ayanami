#include "utils/protocol.hpp"
#include <iostream>
#include <thread>
#include <chrono>

using boost::asio::ip::tcp;

bool reconnect(tcp::socket& socket, boost::asio::io_context& io_context) {
    constexpr int RECONNECT_ATTEMPTS = 5;

    for (int attempt = 1; attempt <= RECONNECT_ATTEMPTS; ++attempt) {
        try {
            socket.close();  // Cerrar el socket anterior
            socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8081));
            std::cout << "Conexión exitosa." << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error al conectar: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Esperar antes de reintentar
        }
    }
    return false; // Si falla después de varios intentos
}

// Construir el mensaje en el formato length:id:content
std::vector<char> build_message(const std::string& id, const std::string& content) {
    uint32_t length = 4 + id.size() + 1 + content.size();
    uint32_t network_length = htonl(length);

    std::vector<char> buffer(length + 4);
    std::memcpy(buffer.data(), &network_length, 4);
    std::memcpy(buffer.data() + 4, id.c_str(), id.size());
    buffer[12] = ':'; // Delimitador entre ID y contenido
    std::memcpy(buffer.data() + 13, content.c_str(), content.size());

    return buffer;
}

// Parsear un mensaje en formato length:id:content
std::pair<std::string, std::string> parse_message(const std::vector<char>& buffer) {
    if (buffer.size() < 4) throw std::runtime_error("Mensaje incompleto");

    uint32_t network_length;
    std::memcpy(&network_length, buffer.data(), 4);
    uint32_t length = ntohl(network_length);

    if (length != buffer.size() - 4) throw std::runtime_error("Longitud incorrecta");

    std::string id(buffer.begin() + 4, buffer.begin() + 12);
    std::string content(buffer.begin() + 13, buffer.end());

    return {id, content};
}

// Leer la respuesta del servidor desde el socket
std::pair<std::string, std::string> read_response(tcp::socket& socket) {
    boost::system::error_code error;

    // Leer la longitud de la respuesta (4 bytes)
    char length_buffer[4];
    boost::asio::read(socket, boost::asio::buffer(length_buffer, 4), error);
    if (error) throw boost::system::system_error(error);

    uint32_t response_length = ntohl(*reinterpret_cast<uint32_t*>(length_buffer));

    // Leer la respuesta completa
    std::vector<char> response_buffer(response_length);
    boost::asio::read(socket, boost::asio::buffer(response_buffer), error);
    if (error) throw boost::system::system_error(error);

    // Convertir la respuesta en un string
    std::string full_response(response_buffer.begin(), response_buffer.end());

    // Encontrar el delimitador ':' para separar el ID y el contenido
    size_t delimiter_pos = full_response.find(':');
    if (delimiter_pos == std::string::npos) {
        throw std::runtime_error("Formato incorrecto: Falta el delimitador ':' en la respuesta.");
    }

    // Separar el ID y la respuesta
    std::string id = full_response.substr(0, delimiter_pos);
    std::string content = full_response.substr(delimiter_pos + 1);

    return {id, content};
}
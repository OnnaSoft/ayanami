#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <locale>

using boost::asio::ip::tcp;

// Tamaño fijo del ID
constexpr size_t FIXED_ID_SIZE = 8;

// Función para eliminar espacios al inicio y al final de una cadena
static inline std::string trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) start++;

    auto end = str.end();
    do { end--; } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

// Función para limpiar bytes nulos
std::string clean_null_terminated(const std::string& input) {
    size_t pos = input.find('\0');
    if (pos != std::string::npos) {
        return input.substr(0, pos);
    }
    return input;
}

// Asegurar que el ID tenga un tamaño fijo de 8 bytes
std::string format_fixed_id(const std::string& id) {
    if (id.size() >= FIXED_ID_SIZE) {
        return id.substr(0, FIXED_ID_SIZE); // Truncar si es demasiado largo
    } else {
        return id + std::string(FIXED_ID_SIZE - id.size(), ' '); // Rellenar con espacios
    }
}

// Procesar el comando
std::string process_command(const std::string& id, const std::string& content) {
    std::ostringstream response;
    std::string cleaned_content = trim(clean_null_terminated(content));

    std::cout << "Comando recibido: " << cleaned_content << " (ID: " << id << ")" << std::endl;

    if (cleaned_content == "PING") {
        response << "PONG";
    } else {
        response << "UNKNOWN COMMAND";
    }

    return response.str();
}

// Función para leer un mensaje completo basado en el formato length:id:content
std::pair<std::string, std::string> read_message(tcp::socket& socket) {
    boost::system::error_code error;

    // Leer los primeros 4 bytes (length)
    char length_buffer[4];
    boost::asio::read(socket, boost::asio::buffer(length_buffer, 4), error);
    if (error) throw boost::system::system_error(error);

    uint32_t message_length = ntohl(*reinterpret_cast<uint32_t*>(length_buffer));

    // Leer el ID (8 bytes)
    char id_buffer[FIXED_ID_SIZE];
    boost::asio::read(socket, boost::asio::buffer(id_buffer, FIXED_ID_SIZE), error);
    if (error) throw boost::system::system_error(error);

    std::string id(id_buffer, FIXED_ID_SIZE);

    // Leer y descartar el delimitador ':'
    char delimiter;
    boost::asio::read(socket, boost::asio::buffer(&delimiter, 1), error);
    if (error) throw boost::system::system_error(error);
    if (delimiter != ':') throw std::runtime_error("Formato incorrecto: Falta el delimitador ':'");

    // Leer el contenido restante
    uint32_t content_length = message_length - FIXED_ID_SIZE - 1; // Restar ID (8 bytes) y ':'
    std::vector<char> content_buffer(content_length);
    boost::asio::read(socket, boost::asio::buffer(content_buffer), error);
    if (error) throw boost::system::system_error(error);

    std::string content(content_buffer.begin(), content_buffer.end());
    return {id, content};
}

int main() {
    try {
        boost::asio::io_context io_context;

        // Escuchar en el puerto 8080
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
        std::cout << "Servidor escuchando en el puerto 8080..." << std::endl;

        for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::cout << "Nueva conexión aceptada." << std::endl;

            try {
                // Leer el mensaje completo
                auto [id, content] = read_message(socket);

                // Asegurar ID con tamaño fijo
                std::string fixed_id = format_fixed_id(id);

                // Procesar el comando
                std::string response_content = process_command(fixed_id, content);

                // Construir la respuesta con ID incluido
                std::string full_response = fixed_id + ":" + response_content;

                uint32_t response_length = htonl(full_response.size());

                // Enviar longitud + respuesta
                std::vector<boost::asio::const_buffer> buffers;
                buffers.push_back(boost::asio::buffer(&response_length, 4));
                buffers.push_back(boost::asio::buffer(full_response));
                boost::asio::write(socket, buffers);

                std::cout << "Respuesta enviada: " << full_response << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error al procesar el mensaje: " << e.what() << std::endl;
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

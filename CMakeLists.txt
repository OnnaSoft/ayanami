cmake_minimum_required(VERSION 3.10)
project(AyanamiBD)

# Configurar estándar C++
set(CMAKE_CXX_STANDARD 17)

# Buscar Boost
find_package(Boost REQUIRED COMPONENTS system)

# Incluir directorios de encabezados
include_directories(${Boost_INCLUDE_DIRS} include)

# Archivos fuente del servidor
set(SERVER_SOURCES
    src/server.cpp
    src/utils/protocol.cpp
    src/utils/strings.cpp
)

# Archivos fuente del cliente
set(CLIENT_SOURCES
    src/client/main.cpp
    src/client/command_handler.cpp
    src/client/ping_worker.cpp
    src/client/receiver.cpp
    src/client/session_manager.cpp
    src/client/history.cpp
    src/utils/protocol.cpp
    src/utils/strings.cpp
)

# Ejecutable del servidor
add_executable(ayanami_server ${SERVER_SOURCES})
target_link_libraries(ayanami_server ${Boost_LIBRARIES})

# Ejecutable del cliente (enlazando readline además de Boost)
add_executable(ayanami_client ${CLIENT_SOURCES})
target_link_libraries(ayanami_client ${Boost_LIBRARIES} readline)

# Mensajes para verificar
message(STATUS "Servidor: ayanami_server configurado.")
message(STATUS "Cliente: ayanami_client configurado con readline.")

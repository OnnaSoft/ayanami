cmake_minimum_required(VERSION 3.15)
project(AyanamiBD)

# Configurar estándar C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF) # Desactivar extensiones específicas del compilador

# Buscar Boost
find_package(Boost 1.70 REQUIRED COMPONENTS system)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost no encontrado. Instala Boost 1.70 o superior.")
endif()

# Mensajes de depuración
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

# Archivos fuente del master
set(MASTER_SOURCES
    src/master/main.cpp
    src/master/handler.cpp
    src/utils/protocol.cpp
    src/utils/strings.cpp
)

# Archivos fuente del nodo
set(NODE_SOURCES
    src/node/main.cpp
    src/node/worker.cpp
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

# Ejecutable del master
add_executable(ayanami_master ${MASTER_SOURCES})
target_include_directories(ayanami_master PRIVATE ${Boost_INCLUDE_DIRS} include)
target_link_libraries(ayanami_master PRIVATE ${Boost_LIBRARIES})

# Ejecutable del nodo
add_executable(ayanami_node ${NODE_SOURCES})
target_include_directories(ayanami_node PRIVATE ${Boost_INCLUDE_DIRS} include)
target_link_libraries(ayanami_node PRIVATE ${Boost_LIBRARIES})

# Ejecutable del cliente
add_executable(ayanami_client ${CLIENT_SOURCES})
target_include_directories(ayanami_client PRIVATE ${Boost_INCLUDE_DIRS} include)
target_link_libraries(ayanami_client PRIVATE ${Boost_LIBRARIES} readline)

# Mensajes para verificar configuración
message(STATUS "Servidor configurado: ayanami_master.")
message(STATUS "Nodo configurado: ayanami_node.")
message(STATUS "Cliente configurado: ayanami_client con readline.")

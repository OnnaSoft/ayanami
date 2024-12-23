cmake_minimum_required(VERSION 3.15)
project(AyanamiBD)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost 1.70 REQUIRED COMPONENTS system)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost no encontrado. Instala Boost 1.70 o superior.")
endif()

include_directories(${Boost_INCLUDE_DIRS})
set(BOOST_LIBS ${Boost_LIBRARIES})

set(MASTER_SOURCES
    src/master/main.cpp
    src/master/config.cpp
    src/master/handler.cpp
    src/utils/protocol.cpp
    src/utils/transport.cpp
    src/utils/strings.cpp
)

set(NODE_SOURCES
    src/node/main.cpp
    src/node/worker.cpp
    src/utils/protocol.cpp
    src/utils/strings.cpp
)

set(CLIENT_SOURCES
    src/client/main.cpp
    src/client/config.cpp
    src/client/command_handler.cpp
    src/client/ping_worker.cpp
    src/client/receiver.cpp
    src/client/session_manager.cpp
    src/client/history.cpp
    src/utils/protocol.cpp
    src/utils/strings.cpp
    src/utils/transport.cpp
)

macro(add_ayanami_executable target sources)
    add_executable(${target} ${sources})
    target_include_directories(${target} PRIVATE ${Boost_INCLUDE_DIRS} include)
    target_link_libraries(${target} PRIVATE ${BOOST_LIBS} readline)
endmacro()

add_ayanami_executable(ayanami_master "${MASTER_SOURCES}")
add_ayanami_executable(ayanami_node "${NODE_SOURCES}")
add_ayanami_executable(ayanami_client "${CLIENT_SOURCES}")

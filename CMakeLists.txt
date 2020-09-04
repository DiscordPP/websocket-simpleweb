cmake_minimum_required(VERSION 3.6)
project(discordpp-websocket-simpleweb)

set(CMAKE_CXX_STANDARD 14)

###################

add_subdirectory(lib/simple-websocket-server)

add_library(${PROJECT_NAME} INTERFACE)

INCLUDE_DIRECTORIES(${discordpp_SOURCE_DIR} ${json_SOURCE_DIR} ${Simple-WebSocket-Server_SOURCE_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} INTERFACE)

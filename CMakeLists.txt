cmake_minimum_required(VERSION 3.6)
project(discordpp-websocket-simpleweb)

set(CMAKE_CXX_STANDARD 14)

###################

add_subdirectory(lib/simple-websocket-server)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${Simple-WebSocket-Server_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE discordpp)

cmake_minimum_required(VERSION 3.15)
project(discordpp-websocket-simpleweb)

set(CMAKE_CXX_STANDARD 14)

###################

list(PREPEND ALL_DISCORDPP_PLUGINS discordpp::WebsocketSimpleWeb)
list(APPEND ALL_DISCORDPP_LIBRARIES discordpp-websocket-simpleweb)
set(ALL_DISCORDPP_LIBRARIES ${ALL_DISCORDPP_LIBRARIES} PARENT_SCOPE)
set(ALL_DISCORDPP_PLUGINS ${ALL_DISCORDPP_PLUGINS} PARENT_SCOPE)

if (NOT ${DISCORDPP_USE_BOOST})
    set(USE_STANDALONE_ASIO ON)
    set(ASIO_PATH ${discordpp_SOURCE_DIR}/lib/asio)
endif ()
add_subdirectory(lib/simple-websocket-server)
if (NOT ${DISCORDPP_USE_BOOST})
    unset(ASIO_PATH)
    unset(USE_STANDALONE_ASIO)
endif ()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${Simple-WebSocket-Server_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE discordpp)

cmake_minimum_required(VERSION 3.20)
project(packet_client)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(packet_client src/client.cpp)

if (WIN32)
    target_link_libraries(packet_client ws2_32)
endif()
cmake_minimum_required(VERSION 2.8.3)
project(linear_sdn)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

add_executable(a3sdn
        src/a3sdn.cpp
    src/packet.cpp
    src/port.cpp
    src/network_node.cpp
    src/controller.cpp
    src/switch.cpp
    src/flow_rule.cpp
)

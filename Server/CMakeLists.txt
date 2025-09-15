cmake_minimum_required(VERSION 3.10)
project(r-type_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Ofast")

include_directories(include)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(r-type_server ${SOURCES})

link_directories(${CMAKE_BINARY_DIR}/libengine)
target_link_libraries(r-type_server PRIVATE engine)
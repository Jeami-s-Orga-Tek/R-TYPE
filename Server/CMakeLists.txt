cmake_minimum_required(VERSION 3.10)
project(r-type_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Ofast")

find_package(Boost REQUIRED COMPONENTS system)

include_directories(include)
include_directories(src)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(r-type_server ${SOURCES})

target_link_libraries(r-type_server PRIVATE Boost::system)
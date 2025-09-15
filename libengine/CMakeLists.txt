cmake_minimum_required(VERSION 3.10)
project(engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Ofast")

include_directories(include)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(engine SHARED ${SOURCES})
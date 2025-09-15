cmake_minimum_required(VERSION 3.10)

project(r-type_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Ofast")
set(SFML_VERSION 2.6.1)

include_directories(include)

file(GLOB SOURCES src/*.cpp)

include(../cmake/CPM.cmake)

CPMAddPackage("gh:sfml/sfml#3.0.0")

add_executable(r-type_client ${SOURCES})

link_directories(${CMAKE_BINARY_DIR}/libengine)
target_link_libraries(r-type_client PRIVATE engine)
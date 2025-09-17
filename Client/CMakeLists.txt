cmake_minimum_required(VERSION 3.10)

project(r-type_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Ofast")

include_directories(include)

file(GLOB SOURCES src/*.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SFML REQUIRED sfml-all)

find_package(Boost REQUIRED COMPONENTS system)

add_executable(r-type_client ${SOURCES})

target_include_directories(r-type_client PRIVATE ${SFML_INCLUDE_DIRS})
target_link_libraries(r-type_client PRIVATE 
    ${SFML_LIBRARIES}
    ${Boost_LIBRARIES}
    pthread
)
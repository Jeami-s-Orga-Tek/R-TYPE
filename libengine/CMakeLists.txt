cmake_minimum_required(VERSION 3.10)
project(engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Ofast")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")

include_directories(../include_common)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(engine SHARED ${SOURCES})


# TEMP REMOVE LATER
# target_link_libraries(engine PRIVATE sfml-graphics sfml-window sfml-system)

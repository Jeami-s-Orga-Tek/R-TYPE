cmake_minimum_required(VERSION 3.10)

option(BUILD_LIBENGINE "Build the libengine library" ON)
option(BUILD_CLIENT "Build the Client executable" ON)
option(BUILD_SERVER "Build the Server executable" OFF)

project(r-type)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

if (BUILD_LIBENGINE)
    add_subdirectory(libengine)
endif(BUILD_LIBENGINE)

if (BUILD_CLIENT)
    add_subdirectory(Client)
endif(BUILD_CLIENT)

if (BUILD_SERVER)
    add_subdirectory(Server)
endif(BUILD_SERVER)

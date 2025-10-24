cmake_minimum_required(VERSION 3.10)

project(r-type)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

option(BUILD_LIBENGINE "Build libengine subdirectory" ON)
option(BUILD_CLIENT "Build Client subdirectory" ON)
option(BUILD_SERVER "Build Server subdirectory" ON)
option(BUILD_PHYSICS_EXAMPLE "Build Example_physics subdirectory" ON)

if(BUILD_LIBENGINE)
    add_subdirectory(libengine)
endif()

if(BUILD_CLIENT)
    add_subdirectory(Client)
endif()

if(BUILD_SERVER)
    add_subdirectory(Server)
endif()

if(BUILD_PHYSICS_EXAMPLE)
    add_subdirectory(physics_example)
endif()

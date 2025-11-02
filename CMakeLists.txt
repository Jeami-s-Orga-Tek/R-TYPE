cmake_minimum_required(VERSION 3.14)
include(cmake/CPM.cmake)
include(FetchContent)

if(NOT DEFINED ENV{CPM_SOURCE_CACHE})
        set(ENV{CPM_SOURCE_CACHE} ${CMAKE_CURRENT_SOURCE_DIR}/.cpm-cache)
endif()

# bad but no other way to do it
if(NOT DEFINED SOL_PATCHED OR NOT SOL_PATCHED)
    set(SOL_PATCHED TRUE CACHE INTERNAL "Whether to apply patches to Sol (workaround to a year old bug yipee)")
    CPMAddPackage(
        NAME sol2
        GITHUB_REPOSITORY ThePhD/sol2
        VERSION 3.3.0
        PATCHES fix_for_clang.patch
    )
else()
    CPMAddPackage(
        NAME sol2
        GITHUB_REPOSITORY ThePhD/sol2
        VERSION 3.3.0
    )
endif()

project(r-type)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

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

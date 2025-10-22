cmake_minimum_required(VERSION 3.10)
project(engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Ofast")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fno-omit-frame-pointer")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -g3 -fno-omit-frame-pointer")

include(../cmake/CPM.cmake)

if(NOT DEFINED ENV{CPM_SOURCE_CACHE})
    set(ENV{CPM_SOURCE_CACHE} ${CMAKE_CURRENT_SOURCE_DIR}/.cpm-cache)
endif()

CPMAddPackage(
  NAME Boost
  VERSION 1.86.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
  URL_HASH SHA256=2c5ec5edcdff47ff55e27ed9560b0a0b94b07bd07ed9928b476150e16b0efc57
  OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON"
          "BUILD_SHARED_LIBS OFF" "BOOST_INCLUDE_LIBRARIES container\\\;asio\\\;pfr\\\;type_index"
)

CPMAddPackage(
  NAME lua
  GITHUB_REPOSITORY lua/lua
  VERSION 5.3.5
  DOWNLOAD_ONLY YES
)

if(lua_ADDED)
  file(GLOB lua_sources ${lua_SOURCE_DIR}/*.c)
  list(REMOVE_ITEM lua_sources "${lua_SOURCE_DIR}/lua.c" "${lua_SOURCE_DIR}/luac.c")
  add_library(lua SHARED ${lua_sources})
  target_include_directories(lua SYSTEM PUBLIC $<BUILD_INTERFACE:${lua_SOURCE_DIR}>)
endif()

if (NOT SOL_PATCHED)
  set(SOL_PATCHED TRUE CACHE INTERNAL "Whether to apply patches to Sol (workaround to a year old bug yipee)")
  CPMAddPackage(
    NAME sol2
    GITHUB_REPOSITORY ThePhD/sol2
    VERSION 3.3.0
    PATCHES ../fix_for_clang.patch
  )
else()
  CPMAddPackage(
    NAME sol2
    GITHUB_REPOSITORY ThePhD/sol2
    VERSION 3.3.0
  )
endif()

include_directories(../include_common)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(engine SHARED ${SOURCES})

target_link_libraries(engine PRIVATE $<$<BOOL:${WIN32}>:ws2_32>)

target_include_directories(engine PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(engine PRIVATE
  Boost::system
  Boost::asio
  Boost::pfr
  Boost::type_index
  pthread
  sol2
  lua
)

# TEMP REMOVE LATER
# target_link_libraries(engine PRIVATE sfml-graphics sfml-window sfml-system)

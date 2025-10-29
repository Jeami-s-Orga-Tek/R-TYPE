
cmake_minimum_required(VERSION 3.10)

project(r-type_client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Ofast")
set(SFML_VERSION 2.6.2)

include_directories(../include_common)

add_subdirectory(../LuaLoader ${CMAKE_CURRENT_BINARY_DIR}/LuaLoader)
include_directories(../LuaLoader)

include_directories(include)

file(GLOB SOURCES src/*.cpp src/Renderers/*.cpp)

include(../cmake/CPM.cmake)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")

if(NOT DEFINED ENV{CPM_SOURCE_CACHE})
    set(ENV{CPM_SOURCE_CACHE} ${CMAKE_CURRENT_SOURCE_DIR}/.cpm-cache)
endif()

CPMAddPackage("gh:sfml/sfml#2.6.2")
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

set(RENDERER "SFML" CACHE STRING "Choose renderer")
set(RENDERER_TARGET "renderer")
set(RENDERER_DIR "Renderers/${RENDERER}")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${RENDERER_DIR}/CMakeLists.txt")
  add_subdirectory(${RENDERER_DIR})
  set(RENDERER_LIB ${RENDERER_TARGET})
else()
  message(FATAL_ERROR "Renderer subdirectory '${RENDERER_DIR}' does not exist.")
endif()

set(AUDIOPLAYER "SFML" CACHE STRING "Choose audioplayer")
set(AUDIOPLAYER_TARGET "audioplayer")
set(AUDIOPLAYER_DIR "AudioPlayers/${AUDIOPLAYER}")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${AUDIOPLAYER_DIR}/CMakeLists.txt")
  add_subdirectory(${AUDIOPLAYER_DIR})
  set(AUDIOPLAYER_LIB ${AUDIOPLAYER_TARGET})
else()
  message(FATAL_ERROR "Audioplayer subdirectory '${AUDIOPLAYER_DIR}' does not exist.")
endif()

set(PHYSICSENGINE "Box2D" CACHE STRING "Choose PHYSICSENGINE")
set(PHYSICSENGINE_TARGET "physicsengine")
set(PHYSICSENGINE_DIR "../PhysicsEngines/${PHYSICSENGINE}")
if(NOT TARGET ${PHYSICSENGINE_TARGET})
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PHYSICSENGINE_DIR}/CMakeLists.txt")
    add_subdirectory(${PHYSICSENGINE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/PhysicsEngines_Box2D)
    set(PHYSICSENGINE_LIB ${PHYSICSENGINE_TARGET})
  else()
    message(FATAL_ERROR "PHYSICSENGINE subdirectory '${CMAKE_CURRENT_SOURCE_DIR}/${PHYSICSENGINE_DIR}' does not exist.")
  endif()
else()
  set(PHYSICSENGINE_LIB ${PHYSICSENGINE_TARGET})
endif()

add_executable(r-type_client ${SOURCES})

target_sources(r-type_client PRIVATE
  src/EditorState.cpp
  include/EditorState.hpp
)

# target_compile_options(r-type_client PRIVATE -g3 -fsanitize=address,leak -fno-omit-frame-pointer)
# target_link_options(r-type_client PRIVATE -g3 -fsanitize=address,leak -fno-omit-frame-pointer)

link_directories(${CMAKE_BINARY_DIR}/libengine)
target_include_directories(r-type_client PRIVATE ${SFML_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(r-type_client PRIVATE
  engine
  LuaLoader
  sfml-graphics
  sfml-window
  sfml-system
  sfml-audio
  ${SFML_LIBRARIES}
  Boost::system
  Boost::asio
  Boost::pfr
  Boost::type_index
  pthread
  sol2
  lua
  ${RENDERER_LIB}
  ${AUDIOPLAYER_LIB}
  ${PHYSICSENGINE_LIB}
)

target_link_libraries(r-type_client PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(r-type_client PRIVATE $<$<BOOL:${WIN32}>:ws2_32>)



cmake_minimum_required(VERSION 3.10)
project(r-type_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Ofast")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fno-omit-frame-pointer")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -g3 -fno-omit-frame-pointer")

include_directories(../include_common)

add_subdirectory(../LuaLoader ${CMAKE_CURRENT_BINARY_DIR}/LuaLoader)
include_directories(../LuaLoader)

include_directories(include)
include_directories(src)

file(GLOB_RECURSE SOURCES src/*.cpp)

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

add_executable(r-type_server ${SOURCES})

target_include_directories(renderer PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(r-type_server PRIVATE engine LuaLoader Boost::system Boost::asio Boost::pfr Boost::type_index sol2 lua ${PHYSICSENGINE_LIB})
target_link_libraries(r-type_server PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(r-type_server PRIVATE $<$<BOOL:${WIN32}>:ws2_32>)

cmake_minimum_required(VERSION 3.10)
project(engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Ofast")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")

include(../cmake/CPM.cmake)

CPMAddPackage(
  NAME Boost
  VERSION 1.86.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
  URL_HASH SHA256=2c5ec5edcdff47ff55e27ed9560b0a0b94b07bd07ed9928b476150e16b0efc57
  OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON"
          "BUILD_SHARED_LIBS OFF" "BOOST_INCLUDE_LIBRARIES container\\\;asio"
)

include_directories(../include_common)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_library(engine SHARED ${SOURCES})

target_include_directories(engine PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(engine PRIVATE
  ${Boost_LIBRARIES}
  pthread
)

# TEMP REMOVE LATER
# target_link_libraries(engine PRIVATE sfml-graphics sfml-window sfml-system)

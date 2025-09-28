cmake_minimum_required(VERSION 3.10)

project(r-type_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Ofast")
set(SFML_VERSION 2.6.2)

include_directories(../include_common)

include_directories(include)

file(GLOB SOURCES src/*.cpp src/net/*.cpp)

include(../cmake/CPM.cmake)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

CPMAddPackage("gh:sfml/sfml#2.6.2")
CPMAddPackage(
  NAME Boost
  VERSION 1.86.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
  URL_HASH SHA256=2c5ec5edcdff47ff55e27ed9560b0a0b94b07bd07ed9928b476150e16b0efc57
  OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON"
          "BUILD_SHARED_LIBS OFF" "BOOST_INCLUDE_LIBRARIES container\\\;asio"
)

add_executable(r-type_client ${SOURCES})

link_directories(${CMAKE_BINARY_DIR}/libengine)
target_include_directories(r-type_client PRIVATE ${SFML_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(r-type_client PRIVATE
  engine
  sfml-graphics
  sfml-window
  sfml-system
  ${SFML_LIBRARIES}
  ${Boost_LIBRARIES}
  pthread
)

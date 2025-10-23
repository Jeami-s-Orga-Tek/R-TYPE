
cmake_minimum_required(VERSION 3.10)

project(physics_example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Ofast")
set(SFML_VERSION 2.6.2)

include_directories(../include_common)
include_directories(include)

file(GLOB SOURCES src/*.cpp src/Renderers/*.cpp)

include(../cmake/CPM.cmake)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")

CPMAddPackage("gh:sfml/sfml#2.6.2")
CPMAddPackage(
  NAME Boost
  VERSION 1.86.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
  URL_HASH SHA256=2c5ec5edcdff47ff55e27ed9560b0a0b94b07bd07ed9928b476150e16b0efc57
  OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON"
          "BUILD_SHARED_LIBS OFF" "BOOST_INCLUDE_LIBRARIES container\\\;asio\\\;pfr\\\;type_index"
)

set(RENDERER "SFML" CACHE STRING "Choose renderer")
set(RENDERER_TARGET "renderer")
set(RENDERER_DIR "../Client/Renderers/${RENDERER}")
if(NOT TARGET ${RENDERER_TARGET})
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${RENDERER_DIR}/CMakeLists.txt")
    add_subdirectory(${RENDERER_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Renderer_SFML)
    set(RENDERER_LIB ${RENDERER_TARGET})
  else()
    message(FATAL_ERROR "Renderer subdirectory '${RENDERER_DIR}' does not exist.")
  endif()
else()
  set(RENDERER_LIB ${RENDERER_TARGET})
endif()

set(AUDIOPLAYER "SFML" CACHE STRING "Choose audioplayer")
set(AUDIOPLAYER_TARGET "audioplayer")
set(AUDIOPLAYER_DIR "../Client/AudioPlayers/${AUDIOPLAYER}")
if(NOT TARGET ${AUDIOPLAYER_TARGET})
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${AUDIOPLAYER_DIR}/CMakeLists.txt")
    add_subdirectory(${AUDIOPLAYER_DIR} ${CMAKE_CURRENT_BINARY_DIR}/AudioPlayer_SFML)
    set(AUDIOPLAYER_LIB ${AUDIOPLAYER_TARGET})
  else()
    message(FATAL_ERROR "Audioplayer subdirectory '${AUDIOPLAYER_DIR}' does not exist.")
  endif()
else()
  set(AUDIOPLAYER_LIB ${AUDIOPLAYER_TARGET})
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

add_executable(physics_example_app ${SOURCES})

link_directories(${CMAKE_BINARY_DIR}/libengine)
target_include_directories(physics_example_app PRIVATE ${SFML_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(physics_example_app PRIVATE
  engine
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
  ${RENDERER_LIB}
  ${AUDIOPLAYER_LIB}
  ${PHYSICSENGINE_LIB}
)

target_link_libraries(physics_example_app PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(physics_example_app PRIVATE $<$<BOOL:${WIN32}>:ws2_32>)



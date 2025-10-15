cmake_minimum_required(VERSION 3.10)

project(r-type_client)

set(CMAKE_CXX_STANDARD 17)

enable_language(ASM C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DKA_SUGGESTED_CXX_FLAGS}")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake-support)
include(Tools3DS)
find_package(CTRULIB REQUIRED)
find_package(CITRO3D REQUIRED)
find_package(CITRO2D REQUIRED)

include_directories(../include_common)

include_directories(include)

file(GLOB SOURCES src/*.cpp src/Renderers/*.cpp)

include(../cmake/CPM.cmake)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -g3 -fsanitize=address,leak -fno-omit-frame-pointer")

# CPMAddPackage(
#   NAME Boost
#   VERSION 1.86.0
#   URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
#   URL_HASH SHA256=2c5ec5edcdff47ff55e27ed9560b0a0b94b07bd07ed9928b476150e16b0efc57
#   OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON"
#           "BUILD_SHARED_LIBS OFF" "BOOST_INCLUDE_LIBRARIES container\\\;asio"
# )

# set(ENV{_3DSXFLAGS} "$ENV{_3DSXFLAGS} --romfs=${CMAKE_BINARY_DIR}/romfs")

set(TEXTURE_DIR ${CMAKE_BINARY_DIR}/assets)
set(TEXTURE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/romfs/gfx)
file(MAKE_DIRECTORY ${TEXTURE_DIR})
file(MAKE_DIRECTORY ${TEXTURE_OUTPUT_DIR})
include_directories(${TEXTURE_DIR})
include_directories(${TEXTURE_OUTPUT_DIR})

target_texture(${TEXTURE_OUTPUT_DIR}/vaisseaux.d ${TEXTURE_OUTPUT_DIR}/vaisseaux.t3x ../../assets/vaisseaux.t3s)
target_bin2o(${TEXTURE_OUTPUT_DIR}/vaisseaux_t3x.o ${TEXTURE_OUTPUT_DIR}/vaisseaux_t3x.h ${TEXTURE_OUTPUT_DIR}/vaisseaux.t3x)
target_texture(${TEXTURE_OUTPUT_DIR}/r-typesheet1.d ${TEXTURE_OUTPUT_DIR}/r-typesheet1.t3x ../../assets/r-typesheet1.t3s)
target_bin2o(${TEXTURE_OUTPUT_DIR}/r-typesheet1_t3x.o ${TEXTURE_OUTPUT_DIR}/r-typesheet1_t3x.h ${TEXTURE_OUTPUT_DIR}/r-typesheet1.t3x)
target_texture(${TEXTURE_OUTPUT_DIR}/r-typesheet7.d ${TEXTURE_OUTPUT_DIR}/r-typesheet7.t3x ../../assets/r-typesheet7.t3s)
target_bin2o(${TEXTURE_OUTPUT_DIR}/r-typesheet7_t3x.o ${TEXTURE_OUTPUT_DIR}/r-typesheet7_t3x.h ${TEXTURE_OUTPUT_DIR}/r-typesheet7.t3x)
target_texture(${TEXTURE_OUTPUT_DIR}/space_background.d ${TEXTURE_OUTPUT_DIR}/space_background.t3x ../../assets/space_background.t3s)
target_bin2o(${TEXTURE_OUTPUT_DIR}/space_background_t3x.o ${TEXTURE_OUTPUT_DIR}/space_background_t3x.h ${TEXTURE_OUTPUT_DIR}/space_background.t3x)

add_executable(r-type_client ${SOURCES}
  ${TEXTURE_OUTPUT_DIR}/vaisseaux_t3x.o ${TEXTURE_OUTPUT_DIR}/vaisseaux_t3x.h
  ${TEXTURE_OUTPUT_DIR}/r-typesheet1_t3x.o ${TEXTURE_OUTPUT_DIR}/r-typesheet1_t3x.h
  ${TEXTURE_OUTPUT_DIR}/r-typesheet7_t3x.o ${TEXTURE_OUTPUT_DIR}/r-typesheet7_t3x.h
  # ${TEXTURE_OUTPUT_DIR}/space_background_t3x.o ${TEXTURE_OUTPUT_DIR}/space_background_t3x.h
)

link_directories(${CMAKE_BINARY_DIR}/libengine)
target_include_directories(r-type_client PRIVATE ${SFML_INCLUDE_DIRS})
target_link_libraries(r-type_client PRIVATE
  engine
  m
  3ds::citro3d
  3ds::ctrulib
  3ds::citro2d
)
target_link_libraries(r-type_client PRIVATE
  engine
  m
  3ds::citro3d
  3ds::ctrulib
  3ds::citro2d
)

add_3dsx_target(r-type_client)
set_property(TARGET r-type_client PROPERTY CXX_STANDARD 17)

# target_link_libraries(r-type_client PRIVATE ${CMAKE_DL_LIBS})

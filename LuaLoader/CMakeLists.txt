
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0079 NEW)

CPMAddPackage("gh:sfml/sfml#2.6.2")
CPMAddPackage(
  NAME Boost
  VERSION 1.86.0
  URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
  URL_HASH SHA256=2c5ec5edcdff47ff55e27ed9560b0a0b94b07bd07ed9928b476150e16b0efc57
  OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON"
          "BUILD_SHARED_LIBS OFF" "BOOST_INCLUDE_LIBRARIES container\\\;asio\\\;pfr\\\;type_index"
)

if(NOT TARGET LuaLoader)
    add_library(LuaLoader STATIC LuaLoader.hpp LuaLoader.cpp)

    target_include_directories(LuaLoader PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    if(TARGET sol2)
        target_link_libraries(LuaLoader PUBLIC sol2)
    endif()
    if(TARGET lua)
        target_link_libraries(LuaLoader PUBLIC lua)
    endif()

    set_target_properties(LuaLoader PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
endif()

target_include_directories(LuaLoader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(LuaLoader PUBLIC
    sol2
    lua
    Boost::system
    Boost::asio
    Boost::pfr
    Boost::type_index
)

set_target_properties(LuaLoader PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
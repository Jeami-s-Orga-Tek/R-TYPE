
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0079 NEW)


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

target_link_libraries(LuaLoader PUBLIC sol2 lua)

set_target_properties(LuaLoader PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
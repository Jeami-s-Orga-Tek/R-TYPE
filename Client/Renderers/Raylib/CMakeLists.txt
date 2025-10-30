set(RAYLIB_RENDERER_SRC
	Raylib.cpp
	Raylib.hpp
)

add_library(renderer SHARED ${RAYLIB_RENDERER_SRC})
target_include_directories(renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ../../../../include_common)

find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

target_link_libraries(renderer PRIVATE
    raylib
    Boost::system
    Boost::asio
    Boost::pfr
    Boost::type_index
    $<$<BOOL:${WIN32}>:ws2_32>
    engine
)
set(RAYLIB_AUDIOPLAYER_SRC
	Raylib.cpp
	Raylib.hpp
)

add_library(audioplayer SHARED ${RAYLIB_AUDIOPLAYER_SRC})
target_include_directories(audioplayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ../../../../include_common)

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

target_link_libraries(audioplayer PRIVATE
    raylib
    Boost::system
    Boost::asio
    Boost::pfr
    Boost::type_index
    $<$<BOOL:${WIN32}>:ws2_32>
    engine
)
set(SDL_AUDIOPLAYER_SRC
	SDL.cpp
	SDL.hpp
)

add_library(audioplayer SHARED ${SDL_AUDIOPLAYER_SRC})
target_include_directories(audioplayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ../../../../include_common)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

target_link_libraries(audioplayer PRIVATE
    SDL2::SDL2
    SDL2_mixer::SDL2_mixer
    Boost::system
    Boost::asio
    Boost::pfr
    Boost::type_index
    $<$<BOOL:${WIN32}>:ws2_32>
    engine
)
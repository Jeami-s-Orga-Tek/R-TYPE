set(SDL_RENDERER_SRC
	SDL.cpp
	SDL.hpp
)

add_library(renderer SHARED ${SDL_RENDERER_SRC})
target_include_directories(renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ../../../../include_common)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_link_libraries(renderer PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    Boost::system
    Boost::asio
    Boost::pfr
    Boost::type_index
    $<$<BOOL:${WIN32}>:ws2_32>
    engine
)
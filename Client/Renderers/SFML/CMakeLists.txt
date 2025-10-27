set(SFML_RENDERER_SRC
	SFML.cpp
	SFML.hpp
)

add_library(renderer SHARED ${SFML_RENDERER_SRC})
target_include_directories(renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ../../../../include_common)
target_link_libraries(renderer PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    $<$<BOOL:${WIN32}>:ws2_32>
    engine
)

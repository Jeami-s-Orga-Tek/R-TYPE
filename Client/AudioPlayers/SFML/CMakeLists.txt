set(SFML_AUDIOPLAYER_SRC
	SFML.cpp
	SFML.hpp
)

add_library(audioplayer SHARED ${SFML_AUDIOPLAYER_SRC})
target_include_directories(audioplayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ../../../../include_common)
target_link_libraries(audioplayer PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    $<$<BOOL:${WIN32}>:ws2_32>
    engine
)

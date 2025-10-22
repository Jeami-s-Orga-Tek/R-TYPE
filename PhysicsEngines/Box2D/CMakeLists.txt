set(BOX2D_PHYSICSENGINE_SRC
	Box2D.cpp
	Box2D.hpp
)

CPMAddPackage(
  NAME box2d
  VERSION 2.4.2
  GITHUB_REPOSITORY erincatto/box2d
  DOWNLOAD_ONLY True
)

add_library(physicsengine SHARED ${BOX2D_PHYSICSENGINE_SRC})
target_include_directories(physicsengine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ../../../../include_common)
target_include_directories(physicsengine PRIVATE ${box2d_INCLUDE_DIRS})
target_link_libraries(physicsengine PRIVATE
    box2d
    $<$<BOOL:${WIN32}>:ws2_32>
)

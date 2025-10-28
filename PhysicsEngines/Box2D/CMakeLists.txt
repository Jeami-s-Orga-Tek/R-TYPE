set(BOX2D_PHYSICSENGINE_SRC
	Box2D.cpp
	Box2D.hpp
)

CPMAddPackage(
  NAME box2d
  VERSION 2.4.2
  GITHUB_REPOSITORY erincatto/box2d
  OPTIONS 
    "BOX2D_BUILD_UNIT_TESTS OFF"
    "BOX2D_BUILD_TESTBED OFF"
)

add_library(physicsengine SHARED ${BOX2D_PHYSICSENGINE_SRC})
target_include_directories(physicsengine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ../../../../include_common)
target_link_libraries(physicsengine PRIVATE
  box2d
  $<$<BOOL:${WIN32}>:ws2_32>
  Boost::system
  Boost::asio
  Boost::pfr
  Boost::type_index
  engine
)

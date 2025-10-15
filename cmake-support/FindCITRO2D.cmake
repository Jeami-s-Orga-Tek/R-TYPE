# Once done this will define
#  CITRO2D_FOUND - System has citro2dlib
#  CITRO2D_INCLUDE_DIRS - The citro2dlib include directories
#  CITRO2D_LIBRARIES - The libraries needed to use citro2dlib
#
# It also adds an imported target named `3ds::citro2d`.
# Linking it is the same as target_link_libraries(target ${CITRO2D_LIBRARIES}) and target_include_directories(target ${CITRO2D_INCLUDE_DIRS}).

# DevkitPro paths are broken on windows, so we have to fix those.
macro (msys_to_cmake_path MsysPath ResultingPath)
    string (REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
endmacro ()

if (NOT DEVKITPRO)
    msys_to_cmake_path("$ENV{DEVKITPRO}" DEVKITPRO)
endif ()

set (CITRO2D_PATHS $ENV{CITRO2D} libcitro2d citro2d ${DEVKITPRO}/libctru)

find_path (
    CITRO2D_INCLUDE_DIR citro2d.h
    PATHS ${CITRO2D_PATHS}
    PATH_SUFFIXES include libctru/include
)

find_library (
    CITRO2D_LIBRARY NAMES citro2d libcitro2d.a
    PATHS ${CITRO2D_PATHS}
    PATH_SUFFIXES lib libctru/lib
)

set (CITRO2D_LIBRARIES ${CITRO2D_LIBRARY})
set (CITRO2D_INCLUDE_DIRS ${CITRO2D_INCLUDE_DIR})

include (FindPackageHandleStandardArgs)
# Handle the QUIETLY and REQUIRED arguments and set CITRO2D_FOUND to TRUE, if all listed variables are TRUE.
find_package_handle_standard_args (
    CITRO2D DEFAULT_MSG CITRO2D_LIBRARY CITRO2D_INCLUDE_DIR
)

mark_as_advanced (CITRO2D_INCLUDE_DIR CITRO2D_LIBRARY)
if (CTRULIB_FOUND)
    set (CITRO2D ${CITRO2D_INCLUDE_DIR}/..)
    message (STATUS "setting CITRO2D to ${CITRO2D}")
    add_library (3ds::citro2d STATIC IMPORTED GLOBAL)
    set_target_properties(
        3ds::citro2d PROPERTIES
        IMPORTED_LOCATION "${CITRO2D_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${CITRO2D_INCLUDE_DIR}"
    )
endif ()


# Once done this will define
#  CITRO3D_FOUND - System has citro3dlib
#  CITRO3D_INCLUDE_DIRS - The citro3dlib include directories
#  CITRO3D_LIBRARIES - The libraries needed to use citro3dlib
#
# It also adds an imported target named `3ds::citro3d`.
# Linking it is the same as target_link_libraries(target ${CITRO3D_LIBRARIES}) and target_include_directories(target ${CITRO3D_INCLUDE_DIRS}).

# DevkitPro paths are broken on windows, so we have to fix those.
macro (msys_to_cmake_path MsysPath ResultingPath)
    string (REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
endmacro ()

if (NOT DEVKITPRO)
    msys_to_cmake_path("$ENV{DEVKITPRO}" DEVKITPRO)
endif ()

set (CITRO3D_PATHS $ENV{CITRO3D} libcitro3d citro3d ${DEVKITPRO}/libctru)

find_path (
    CITRO3D_INCLUDE_DIR citro3d.h
    PATHS ${CITRO3D_PATHS}
    PATH_SUFFIXES include libctru/include
)

find_library (
    CITRO3D_LIBRARY NAMES citro3d libcitro3d.a
    PATHS ${CITRO3D_PATHS}
    PATH_SUFFIXES lib libctru/lib
)

set (CITRO3D_LIBRARIES ${CITRO3D_LIBRARY})
set (CITRO3D_INCLUDE_DIRS ${CITRO3D_INCLUDE_DIR})

include (FindPackageHandleStandardArgs)
# Handle the QUIETLY and REQUIRED arguments and set CITRO3D_FOUND to TRUE, if all listed variables are TRUE.
find_package_handle_standard_args (
    CITRO3D DEFAULT_MSG CITRO3D_LIBRARY CITRO3D_INCLUDE_DIR
)

mark_as_advanced (CITRO3D_INCLUDE_DIR CITRO3D_LIBRARY)
if (CTRULIB_FOUND)
    set (CITRO3D ${CITRO3D_INCLUDE_DIR}/..)
    message (STATUS "setting CITRO3D to ${CITRO3D}")
    add_library (3ds::citro3d STATIC IMPORTED GLOBAL)
    set_target_properties(
        3ds::citro3d PROPERTIES
        IMPORTED_LOCATION "${CITRO3D_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${CITRO3D_INCLUDE_DIR}"
    )
endif ()

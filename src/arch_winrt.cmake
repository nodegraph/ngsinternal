
message("ARCH_BITS for winrt is: ${ARCH_BITS}")

# ---------------------------------------------------------
# Qt Directories.
# --------------------------------------------------------
if (${ARCH_BITS} STREQUAL "x64")
	set(QT5_DIR "D:/installs/winrt/Qt5.6.0/5.6/winrt_x64_msvc2015")
else()
	set(QT5_DIR "D:/installs/winrt/Qt5.6.0/5.6/winrt_x86_msvc2015")
endif()

# Debug Settings.
if ("${CMAKE_BUILD_TYPE}" STREQUAL Debug)
    add_definitions("-DQT_DECLARATIVE_DEBUG")
    add_definitions("-DQT_QML_DEBUG")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
endif ()

# ---------------------------------------------------------
# Our Directories.
# ---------------------------------------------------------
set(FREETYPE_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/vs2015/freetype-2.5.2")
set(GLEW_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/vs2015/glew-1.13.0")
set(GLM_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/vs2015/glm-0.9.5.4")
set(FREETYPE_GL_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/vs2015/freetype-gl")

message("src root is set to: ${SRC_ROOT}")

# Find qt library.
set(CMAKE_PREFIX_PATH ${QT5_DIR})
find_package(Qt5Core)
find_package(Qt5Widgets)

# Setup up our cpp flags.
include_directories("${QT5_DIR}/include")
include_directories("${FREETYPE_DIR}/include")
include_directories("${GLEW_DIR}/include")
include_directories("${GLM_DIR}")
include_directories("${FREETYPE_GL_DIR}/include")
#include_directories("${ANGLE_DIR}/Include")
include_directories("${QT5_DIR}/include/QtANGLE")

# Setup up our link flags.
link_directories("${QT5_DIR}/lib")
link_directories("${QT5_DIR}/bin")

if (${ARCH_BITS} STREQUAL "x64")
	link_directories("${FREETYPE_DIR}/lib/win64")
	link_directories("${FREETYPE_GL_DIR}/lib/win64")
else ()
	link_directories("${FREETYPE_DIR}/lib/win32")
	link_directories("${FREETYPE_GL_DIR}/lib/win32")
endif ()



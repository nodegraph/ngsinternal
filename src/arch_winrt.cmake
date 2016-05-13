
message("ARCH_BITS for winrt is: ${ARCH_BITS}")

# ---------------------------------------------------------
# OpenGL Settings.
# --------------------------------------------------------

# GLES_MAJOR_VERSION AND GLES_MINOR_VERSION define which version of opengl es to use.
# When the GLES_MAJOR_VERSION is greater than 100, it is assumed to mean desktop opengl 4.0 and greater.

set(gles_major_version 2)
set(gles_minor_version 0)
set(gles_use_angle 1)

add_definitions("-DGLES_MAJOR_VERSION=${gles_major_version}")
add_definitions("-DGLES_MINOR_VERSION=${gles_minor_version}")
add_definitions("-DGLES_USE_ANGLE=${gles_use_angle}")

# ---------------------------------------------------------
# Qt Directories.
# --------------------------------------------------------
if (${ARCH_BITS} STREQUAL "x64")
    message("aaaaaaaaaaaaaaa x64")
	set(QT5_DIR "D:/installs/winrt/Qt5.6.0/5.6/winrt_x64_msvc2015")
else()
    message("bbbbbbbbbbbbbb x86")
	set(QT5_DIR "D:/installs/winrt/Qt5.6.0/5.6/winrt_x86_msvc2015")
endif()

# ---------------------------------------------------------
# Our Directories.
# ---------------------------------------------------------
set(FREETYPE_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/vs2015/freetype-2.5.2")
set(GLEW_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/vs2015/glew-1.13.0")
set(GLM_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/vs2015/glm-0.9.5.4")
set(FREETYPE_GL_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/vs2015/freetype-gl")
set(LIBSODIUM_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/vs2015/libsodium-1.0.10")

message("src root is set to: ${SRC_ROOT}")

# Setup up our cpp flags.
include_directories("${QT5_DIR}/include")
include_directories("${FREETYPE_DIR}/include")
include_directories("${GLEW_DIR}/include")
include_directories("${GLM_DIR}")
include_directories("${FREETYPE_GL_DIR}/include")
include_directories("${QT5_DIR}/include/QtANGLE")
include_directories("${LIBSODIUM_DIR}/include")

# Setup up our link flags.
link_directories("${QT5_DIR}/lib")
link_directories("${QT5_DIR}/bin")

if (${ARCH_BITS} STREQUAL "x64")
	link_directories("${FREETYPE_DIR}/lib/win64")
	link_directories("${FREETYPE_GL_DIR}/lib/win64")
    link_directories("${LIBSODIUM_DIR}/x64/${CMAKE_BUILD_TYPE}/v140/static")
else ()
	link_directories("${FREETYPE_DIR}/lib/win32")
	link_directories("${FREETYPE_GL_DIR}/lib/win32")
    link_directories("${LIBSODIUM_DIR}/Win32/${CMAKE_BUILD_TYPE}/v140/static")
endif ()



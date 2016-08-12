# ---------------------------------------------------------
# OpenGL Settings.
# --------------------------------------------------------

# GLES_MAJOR_VERSION AND GLES_MINOR_VERSION define which version of opengl es to use.
# When the GLES_MAJOR_VERSION is greater than 100, it is assumed to mean desktop opengl 4.0 and greater.

set(gles_major_version 3)
set(gles_minor_version 0)
set(gles_use_angle 0)

add_definitions("-DGLES_MAJOR_VERSION=${gles_major_version}")
add_definitions("-DGLES_MINOR_VERSION=${gles_minor_version}")
add_definitions("-DGLES_USE_ANGLE=${gles_use_angle}")

# ---------------------------------------------------------
# Check our toolchain cmake variables.
# ---------------------------------------------------------

# The android toolchain should have already grabbed these
# environment variables from the environment.

# debugging
#message("XXX android sdk root is: ${ANDROID_SDK_ROOT}")
#message("XXX android ndk root is: ${ANDROID_NDK_ROOT}")
#message("XXX android toolchain root is: ${ANDROID_TOOLCHAIN_ROOT}")

if (NOT ANDROID_SDK_ROOT)
    message(FATAL_ERROR "ANDROID_SDK_ROOT was not defined in shell!")
endif()
if (NOT ANDROID_NDK_ROOT)
    message(FATAL_ERROR "ANDROID_NDK_ROOT was not defined in shell!")
endif()
if (NOT ANDROID_TOOLCHAIN_ROOT)
    message(FATAL_ERROR "ANDROID_TOOLCHAIN_ROOT was not defined in shell!")
endif()

# ---------------------------------------------------------
# Qt Directory.
# ---------------------------------------------------------

set(QT5_DIR "D:/installs/android/Qt5.7.0/5.7/android_armv7")

# ---------------------------------------------------------
# Our directories.
# ---------------------------------------------------------

set(FREETYPE_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/android/freetype-2.5.2")
set(GLM_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/android/glm-0.9.5.4")
set(FREETYPE_GL_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/android/freetype-gl")
set(LIBSODIUM_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/android/libsodium-1.0.10/libsodium-android-armv7-a")

# Global Properties.
SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

# Setup up our cpp flags.
include_directories("${QT5_DIR}/include")
include_directories("${ANDROID_TOOLCHAIN_ROOT}/system/usr/include")
include_directories("${FREETYPE_DIR}/include")
include_directories("${FREETYPE_GL_DIR}/include")
include_directories("${GLM_DIR}")
include_directories("${LIBSODIUM_DIR}/include")

# Setup up our link flags.
link_directories("${QT5_DIR}/lib")
link_directories("${PROJECT_BINARY_DIR}")
link_directories("${ANDROID_TOOLCHAIN_ROOT}/system/usr/lib}") # this is where we find libGLESv3.so
link_directories("${ANDROID_NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a") # this is where we find libgnustl_shared.so
link_directories("${FREETYPE_DIR}/lib")
link_directories("${FREETYPE_GL_DIR}/lib")
link_directories("${LIBSODIUM_DIR}/lib")



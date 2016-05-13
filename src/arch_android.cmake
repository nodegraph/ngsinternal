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

#set(QT5_DIR "D:/installs/android/Qt5.5.1/5.5/android_armv7")
set(QT5_DIR "D:/installs/android/Qt5.6.0/5.6/android_armv7")



# ---------------------------------------------------------
# Custom Directories.
# ---------------------------------------------------------


# ---------------------------------------------------------
# Our directories.
# ---------------------------------------------------------

# Global Properties.
SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

# Setup up our cpp flags.
include_directories("${QT5_DIR}/include")
include_directories("${ANDROID_TOOLCHAIN_ROOT}/system/usr/include")
include_directories("${PLATFORM_ROOT}/srcdeps/ngsexternal/android/freetype-2.5.2/include")
include_directories("${PLATFORM_ROOT}/srcdeps/ngsexternal/android/freetype-gl/include")
include_directories("${PLATFORM_ROOT}/srcdeps/ngsexternal/android/glm-0.9.5.4")
include_directories("${PLATFORM_ROOT}/srcdeps/ngsexternal/android/libsodium-1.0.10/libsodium-android-armv7-a/include")

# Setup up our link flags.
link_directories("${QT5_DIR}/lib")
link_directories("${PROJECT_BINARY_DIR}")
link_directories("${ANDROID_TOOLCHAIN_ROOT}/system/usr/lib}") # this is where we find libGLESv3.so
link_directories("${ANDROID_NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a") # this is where we find libgnustl_shared.so
link_directories("${PLATFORM_ROOT}/srcdeps/ngsexternal/android/freetype-2.5.2/lib")
link_directories("${PLATFORM_ROOT}/srcdeps/ngsexternal/android/freetype-gl/lib")
link_directories("${PLATFORM_ROOT}/srcdeps/ngsexternal/android/libsodium-1.0.10/libsodium-android-armv7-a/lib")



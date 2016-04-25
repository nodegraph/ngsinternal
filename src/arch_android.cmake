# ---------------------------------------------------------
# Check our toolchain cmake variables.
# ---------------------------------------------------------

# Grab some variables from our environment.
# Switch backslashes to forward slashes in SRC_ROOT.
string(REPLACE "\\" "/" SRC_ROOT $ENV{SRC_ROOT})
message("src root is set to: ${SRC_ROOT}")

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

# Debug Settings.
if ("${CMAKE_BUILD_TYPE}" STREQUAL Debug)
    add_definitions("-DQT_DECLARATIVE_DEBUG")
    add_definitions("-DQT_QML_DEBUG")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
endif ()

# ---------------------------------------------------------
# Custom Directories.
# ---------------------------------------------------------


# ---------------------------------------------------------
# Our directories.
# ---------------------------------------------------------

# Global Properties.
SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

# Find qt library.
set(CMAKE_PREFIX_PATH ${QT5_DIR})
find_package(Qt5Core)
find_package(Qt5Widgets)

# Setup up our cpp flags.
include_directories("${QT5_DIR}/include")
include_directories("${ANDROID_TOOLCHAIN_ROOT}/system/usr/include")
include_directories("${SRC_ROOT}/ngsexternal/android/freetype-2.5.2/include")
include_directories("${SRC_ROOT}/ngsexternal/android/freetype-gl/include")
include_directories("${SRC_ROOT}/ngsexternal/android/glm-0.9.5.4")

# Setup up our link flags.
link_directories("${QT5_DIR}/lib")
link_directories("${PROJECT_BINARY_DIR}")
link_directories("${ANDROID_TOOLCHAIN_ROOT}/system/usr/lib}") # this is where we find libGLESv3.so
link_directories("${ANDROID_NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a") # this is where we find libgnustl_shared.so
link_directories("${SRC_ROOT}/ngsexternal/android/freetype-2.5.2/lib")
link_directories("${SRC_ROOT}/ngsexternal/android/freetype-gl/lib")



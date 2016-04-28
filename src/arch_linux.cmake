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
# Our custom cmake variables.
# ---------------------------------------------------------

set(QT5_DIR "C:\Qt\Qt5.5.1\5.5\msvc2013_64")
set(FREETYPE_DIR "/home/raindrop/dev/local/freetype-2.5.2")
set(GLEW_DIR "/home/raindrop/dev/local/glew-1.13.0")

# Opengl 4.0 is available on the desktops. To use it
# set the GLES_MAJOR_VERSION variable to 100.
# This is defined in the main CMakeLists.txt file and can be set from cmake-gui.

# ---------------------------------------------------------
# Our build setup for linux.
# ---------------------------------------------------------

# Enable C++11 features. (-std=gnu++0x)  -std=c++11 
# Hide symbols unless explicity exported from libraries. (fvisibility=hidden)
# Make sure there are no undefined symbols left when linking to create a shared library.   

# Debug settings.
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wl,-z,defs -Wl,--no-undefined -fvisibility=hidden -fPIC -std=gnu++11 ") # CACHE STRING "Debug options." FORCE)
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g -Wl,-z,defs -Wl,--no-undefined -fvisibility=hidden -fPIC"              ) # CACHE STRING "Debug options." FORCE)

# Release settings.
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wl,-z,defs -Wl,--no-undefined -fvisibility=hidden -fPIC -std=gnu++11 ") # CACHE STRING " options." FORCE)
set(CMAKE_C_FLAGS_RELEASE   "-O3 -Wl,-z,defs -Wl,--no-undefined -fvisibility=hidden -fPIC"              ) # CACHE STRING "Debug options." FORCE)

# Add glew defines.
add_definitions("-DGLEW_MX")

# Find qt library.
#set(CMAKE_PREFIX_PATH ${QT5_DIR})
#find_package(Qt5Core)
#find_package(Qt5Widgets)

# Setup up our cpp flags.
include_directories("${QT5_DIR}/include")
include_directories("${QT5_DIR}/include/QtCore")
#include_directories("${QT5_DIR}/include/QtCore/5.5.0") # so that we can use stuff in QtGui private.
#include_directories("${QT5_DIR}/include/QtCore/5.5.0/QtCore")
#include_directories("${QT5_DIR}/include/QtGui")
#include_directories("${QT5_DIR}/include/QtGui/5.5.0/QtGui")
#include_directories("/home/raindrop/dev/local/Qt5.5.0/5.5/gcc_64/mkspecs/linux-g++-64")

include_directories("${FREETYPE_DIR}/include/freetype2")
include_directories("${GLEW_DIR}/include")

# Setup up our link flags.
link_directories("/usr/lib64")
link_directories("${QT5_DIR}/lib")
link_directories("${FREETYPE_DIR}/lib")
link_directories("${GLEW_DIR}/lib64")


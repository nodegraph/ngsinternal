
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
# Qt Directory.
# ---------------------------------------------------------

set(QT5_DIR "/Users/raindrop/installs/macos/Qt5.7.0/5.7/clang_64")

# ---------------------------------------------------------
# Custom Directories. 
# ---------------------------------------------------------

set(FREETYPE_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/macos/freetype-2.5.2")
set(GLEW_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/macos/glew-1.13.0")
set(GLM_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/macos/glm-0.9.5.4")
set(FREETYPE_GL_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/macos/freetype-gl")
set(LIBSODIUM_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/macos/libsodium-1.0.10")

# ---------------------------------------------------------
# Our build setup for macos.
# ---------------------------------------------------------

# Enable C++11 features. (-std=gnu++0x)  -std=c++11 
# Hide symbols unless explicity exported from libraries. (fvisibility=hidden)
# Make sure there are no undefined symbols left when linking to create a shared library.   

# Debug settings.
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fvisibility=hidden -fPIC -std=gnu++11 ") # CACHE STRING "Debug options." FORCE)
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g -fvisibility=hidden -fPIC"              ) # CACHE STRING "Debug options." FORCE)

# Release settings.
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fvisibility=hidden -fPIC -std=gnu++11 ") # CACHE STRING " options." FORCE)
set(CMAKE_C_FLAGS_RELEASE   "-O3 -fvisibility=hidden -fPIC"              ) # CACHE STRING "Debug options." FORCE)

# Add glew defines.
add_definitions("-DGLEW_MX")

# Setup up our cpp flags.
include_directories("${FREETYPE_DIR}/include")
include_directories("${GLEW_DIR}/include")
include_directories("${GLM_DIR}")
include_directories("${FREETYPE_GL_DIR}/include")
include_directories("${LIBSODIUM_DIR}/include")

# GL headers are here on macos
include_directories("/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/System/Library/Frameworks/OpenGL.framework/Headers")

# Setup up our link flags.
link_directories("/usr/lib64")
link_directories("${QT5_DIR}/lib")
link_directories("${FREETYPE_DIR}/lib")
link_directories("${FREETYPE_GL_DIR}/lib")
link_directories("${GLEW_DIR}/lib")
link_directories("${LIBSODIUM_DIR}/lib")
# GL libraries are here on macos
link_directories("/System/Library/Frameworks/OpenGL.framework/Libraries") 


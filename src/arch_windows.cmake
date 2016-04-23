# ---------------------------------------------------------
# Our custom cmake variables.
# ---------------------------------------------------------

# Grab some variables from our environment.
# Switch backslashes to forward slashes in DEV_ROOT.
string(REPLACE "\\" "/" DEV_ROOT $ENV{DEV_ROOT})
message("dev root is set to: ${DEV_ROOT}")

# ---------------------------------------------------------
# Qt Directory.
# ---------------------------------------------------------

#set(QT5_DIR "C:/Qt/Qt5.5.1/5.5/msvc2013_64")
set(QT5_DIR "D:/installs/windows/Qt5.6.0/5.6/msvc2015_64")

# ---------------------------------------------------------
# Custom Directories.
# ---------------------------------------------------------

set(FREETYPE_DIR "${DEV_ROOT}/nodegraphexternal/vs2015/freetype-2.5.2")
set(GLEW_DIR "${DEV_ROOT}/nodegraphexternal/vs2015/glew-1.13.0")
set(GLM_DIR "${DEV_ROOT}/nodegraphexternal/vs2015/glm-0.9.5.4")
set(FREETYPE_GL_DIR "${DEV_ROOT}/nodegraphexternal/vs2015/freetype-gl")

# ---------------------------------------------------------
# Our build setup for windows.
# ---------------------------------------------------------

# /wd4251 disables warning about objects needing a dll interface 
# /wd4351 disables warning about msvc new behavior for default initializing an array
# /MD chooses the c runtime
# /Ob1 sets inline function expansion 
# /Od suppresses code movement to make debugging easier
# /Zi sets the debug information format
# /MP allows parallel building of source files
# /Gm- disable minimal rebuild - not compatible with parallel building?
# /GL- disalbe whole program optimization
# /P ouputs preprocessed file as *.i files somewhere in the build are - helps when debugging macros.
# /W3 set the warning level to 3
# /w34061 set the C4061 warning to report at level 3

# Debug compiler flags.
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Ob0 /Od /Debug /W3 /Zi /MP /wd4251 /wd4351") # CACHE STRING "Debug options." FORCE)
set(CMAKE_C_FLAGS_DEBUG   "/MDd /Ob0 /Od /Debug /W3 /Zi /MP /wd4251 /wd4351") # CACHE STRING "Debug options." FORCE)

# Release compiler flags.
set(CMAKE_CXX_FLAGS_RELEASE "/MD /Ob2 /O2 /MP /wd4251 /wd4351")
set(CMAKE_C_FLAGS_RELEASE   "/MD /Ob2 /O2 /MP /wd4251 /wd4351")

# Add glew defines.
add_definitions("-DGLEW_MX")

# Find qt library.
set(CMAKE_PREFIX_PATH ${QT5_DIR})
find_package(Qt5Core)
find_package(Qt5Widgets)

# Setup up our cpp flags.
include_directories("${QT5_DIR}/include")
include_directories("${FREETYPE_DIR}/include")
include_directories("${GLM_DIR}")
include_directories("${FREETYPE_GL_DIR}/include")

if (${gles_use_angle} STREQUAL "1")
	message("arch windows is using angle GL includes")
	include_directories("${QT5_DIR}/include/QtANGLE")
else()
	message("arch windows is using gles GL includes!")
	include_directories("${GLEW_DIR}/include")
endif()

# Setup up our link flags.
link_directories("${QT5_DIR}/lib")
link_directories("${FREETYPE_DIR}/lib/win64")
link_directories("${FREETYPE_GL_DIR}/lib/win64")

if (NOT(${gles_use_angle} STREQUAL "1"))
	link_directories("${GLEW_DIR}/lib/Debug MX/x64")
	link_directories("${GLEW_DIR}/lib/Release MX/x64")
endif()


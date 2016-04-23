
# ---------------------------------------------------------
# Our custom cmake variables.
# ---------------------------------------------------------

# Grab some variables from our environment.
# Switch backslashes to forward slashes in DEV_ROOT.
string(REPLACE "\\" "/" DEV_ROOT $ENV{DEV_ROOT})

# ---------------------------------------------------------
# Qt Directories.
# --------------------------------------------------------
#set(QT5_DIR "D:/installs/winrt/Qt5.5.1/5.5/winrt_x64")
set(QT5_DIR "D:/installs/winrt/Qt5.6.0/5.6/winrt_x64_msvc2015")

# Debug Settings.
if ("${CMAKE_BUILD_TYPE}" STREQUAL Debug)
    add_definitions("-DQT_DECLARATIVE_DEBUG")
    add_definitions("-DQT_QML_DEBUG")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
endif ()

# ---------------------------------------------------------
# Our Directories.
# ---------------------------------------------------------
set(FREETYPE_DIR "${DEV_ROOT}/nodegraphexternal/vs2015/freetype-2.5.2")
set(GLEW_DIR "${DEV_ROOT}/nodegraphexternal/vs2015/glew-1.13.0")
set(GLM_DIR "${DEV_ROOT}/nodegraphexternal/vs2015/glm-0.9.5.4")
set(FREETYPE_GL_DIR "${DEV_ROOT}/nodegraphexternal/vs2015/freetype-gl")
#set(ANGLE_DIR "${DEV_ROOT}/nodegraphexternal/nuget/angle.windowsstore.2.1.8")

message("dev root is set to: ${DEV_ROOT}")

# Opengl 4.0 is available on the desktops. To use it
# set the GLES_MAJOR_VERSION variable to 100.
# This is defined in the main CMakeLists.txt file and can be set from cmake-gui.

# Add glew defines.
add_definitions("-DGLEW_MX")

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
link_directories("${FREETYPE_DIR}/lib/win64")
link_directories("${GLEW_DIR}/lib/Debug MX/x64")
link_directories("${GLEW_DIR}/lib/Relase MX/x64")
link_directories("${FREETYPE_GL_DIR}/lib/win64")
#link_directories("${ANGLE_DIR}/bin/Windows/x64")




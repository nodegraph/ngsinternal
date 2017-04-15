# OPEN NODE GRAPH
cmake_minimum_required (VERSION 2.8.10)

project (ngs)
include (GenerateExportHeader)

# Grab some variables from our environment.
# Switch backslashes to forward slashes in SRC_ROOT.

string(REPLACE "\\" "/" SRC_ROOT $ENV{SRC_ROOT})
string(REPLACE "\\" "/" PLATFORM_ROOT $ENV{PLATFORM_ROOT})
message("SRC_ROOT is: ${SRC_ROOT}")
message("PLATFORM_ROOT is: ${PLATFORM_ROOT}")

set(PASSWORD $ENV{PASSWORD})
set(ARCH_BITS $ENV{ARCH_BITS})

# The INSTALL command won't keep the directory structure.
# So we use this hack  
function (install_headers headers dest)
	foreach(h ${headers})
	    string(REGEX MATCH "(.*)[/\\]" dir ${h})
	    install(FILES ${h} DESTINATION ${dest}/${dir})
	endforeach(h ${headers})
endfunction()

# The version number.
set(ngs_version_major 1)
set(ngs_version_minor 15)
set(ngs_version_patch 3)
set(ngs_version_tweak 2)
set(ngs_version ${ngs_version_major}.${ngs_version_minor}.${ngs_version_patch}.${ngs_version_tweak})

# Our supported archictectures.
set(karch_linux 0)
set(karch_windows 1)
set(karch_winrt 2)
set(karch_android 3)
set(karch_macos 4)
set(karch_ios 5)

# Architecture Defines.
add_definitions("-DARCH_LINUX=${karch_linux}")
add_definitions("-DARCH_WINDOWS=${karch_windows}")
add_definitions("-DARCH_WINRT=${karch_winrt}")
add_definitions("-DARCH_ANDROID=${karch_android}")
add_definitions("-DARCH_MACOS=${karch_macos}")
add_definitions("-DARCH_IOS=${karch_ios}")

# Force Qt's scene graph rendering to happen in the main thread.
# add_definitions("-DQSG_RENDER_LOOP=basic")

# BUILD_TYPE refers to the type of build.
# DEBUG_TYPE
# RELEASE_TYPE
set(kdebug 0)
set(krelease 1)
add_definitions("-DDEBUG_TYPE=${kdebug}")
add_definitions("-DRELEASE_TYPE=${krelease}")

# Various Build Settings Follow.
# Debug the object model.
add_definitions("-DDEBUG_OBJECT_MODEL")
add_definitions("-DDEBUG_MEMORY_ALLOCATION")
add_definitions("-DMULTITHREAD_COMPONENTS")
# skip license check and skip encryption should be enabled/disabled together, 
# because skipping license check also skips password entry in qml.
#add_definitions("-DSKIP_LICENSE_CHECK") 
#add_definitions("-DSKIP_ENCRYPTION")


# Debug binaries will not have d as a postfix, because this makes our scripts have special cases.
# set(CMAKE_DEBUG_POSTFIX d)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "the install prefix" FORCE)
endif()

# By default the ARCH is set to windows.
# When using a cmake toolchain file, that file will set the ARCH.
set(ARCH "ARCH_WINDOWS" CACHE STRING "ARCH_LINUX, ARCH_WINDOWS, ARCH_WINRT, ARCH_ANDROID, ARCH_MACOS, ARCH_IOS")
message("building for ${ARCH}")

#set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build CACHE PATH "Library output path")
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build CACHE PATH "Executable output path")

# Determine where we want to install to.
set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/install" CACHE PATH "Choose the install location.")

# Determine what type of build to perform.
# Note only Debug and Release are supported on Android builds, because we use the android-cmake utility.
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# The build_type variable is just an all lowercase version of CMAKE_BUILD_TYPE.
if ("${CMAKE_BUILD_TYPE}" STREQUAL Debug)
    add_definitions("-DBUILD_TYPE=${kdebug}")
    set(build_type "debug")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
    add_definitions("-DBUILD_TYPE=${krelease}")
    set(build_type "release")
endif ()

# We build shared libs by default.
set(BUILD_SHARED_LIBS TRUE)

# General includes.
include_directories("${PROJECT_BINARY_DIR}") # allows us to find NodeGraphVersion.h
include_directories("${PROJECT_SOURCE_DIR}")

# Our modified third party software includes.
include_directories("${PROJECT_SOURCE_DIR}/external/half")
include_directories("${PROJECT_SOURCE_DIR}/external/boost-1.59.0")
include_directories("${PROJECT_SOURCE_DIR}/external/chromium")
include_directories("${PROJECT_SOURCE_DIR}/external")

# General lib dirs.
link_directories("${PROJECT_BINARY_DIR}")


# ---------------------------------------------------------
# Closure Compiler
# ---------------------------------------------------------
set(tsconfig_json "tsconfig.json")

# ---------------------------------------------------------
# Closure Compiler
# ---------------------------------------------------------
set(closure_compiler_jar "${PLATFORM_ROOT}/srcdeps/ngsexternal/closure_compiler/closure-compiler-v20170218.jar")




 
# Load the architecture specific cmake file.
if (${ARCH} STREQUAL ARCH_LINUX)
	add_definitions("-DARCH=${karch_linux}")
	include("./arch_linux.cmake")	
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
	add_definitions("-DARCH=${karch_windows}")
	include("./arch_windows.cmake")
elseif (${ARCH} STREQUAL ARCH_WINRT)
	add_definitions("-DARCH=${karch_winrt}")
	include("./arch_winrt.cmake")
elseif (${ARCH} STREQUAL ARCH_ANDROID)
	add_definitions("-DARCH=${karch_android}")
	include("./arch_android.cmake")
elseif (${ARCH} STREQUAL ARCH_MACOS)
	add_definitions("-DARCH=${karch_macos}")
	include("./arch_macos.cmake")
elseif (${ARCH} STREQUAL ARCH_IOS)
	add_definitions("-DARCH=${karch_ios}")
	include("./arch_ios.cmake")
else ()
	message("error: unknown architecture specified")
endif ()

# Create the node graph version header file.
configure_file (
    "${PROJECT_SOURCE_DIR}/ngsversion.h.in"
    "${PROJECT_BINARY_DIR}/ngsversion.h"
  )
  
configure_file (
    "${PROJECT_SOURCE_DIR}/ngsversion.sh.in"
    "${PROJECT_BINARY_DIR}/ngsversion.sh"
  )
  
# Find qt library.
set(CMAKE_PREFIX_PATH ${QT5_DIR} /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/System/Library/Frameworks/OpenGL.framework/Headers)
find_package(Qt5Core)
find_package(Qt5Widgets)

find_package(Qt5OpenGL)
find_package(Qt5OpenGLExtensions)

find_package(Qt5WebView)
find_package(Qt5WebChannel)
find_package(Qt5WebSockets)
find_package(Qt5Network)

if (${ARCH} STREQUAL "ARCH_ANDROID")
	find_package(Qt5AndroidExtras)
elseif (${ARCH} STREQUAL "ARCH_IOS")
	find_package(Qt5DBus)
endif()

find_package(Qt5Qml)
find_package(Qt5Xml)
find_package(Qt5Quick)
find_package(Qt5QuickWidgets)
find_package(Qt5Multimedia)
find_package(Qt5MultimediaWidgets)

find_package(Qt5Widgets)
find_package(Qt5Quick)
find_package(Qt5Gui)
find_package(Qt5Qml)
find_package(Qt5Network)
find_package(Qt5Core)


# Debug Settings.
if ("${CMAKE_BUILD_TYPE}" STREQUAL Debug)
    #add_definitions("-DQT_DECLARATIVE_DEBUG")
    #add_definitions("-DQT_QML_DEBUG")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
endif ()

#get_target_property(QtCore_location Qt5::Core LOCATION)
#message("the qt core location is: ${QtCore_location}")


# Set policy to automatically link qtmain.lib into executables.  
cmake_policy(SET CMP0020 NEW)

# Set policy to use rpaths.
cmake_policy(SET CMP0042 NEW)

# Set policy to ignore the old settings of COMPILE_DEFINITIONS_<CONFIG>.
cmake_policy(SET CMP0043 NEW)

# Check the chosen gl version.
message("gl version: ${gles_major_version}.${gles_minor_version}")
message("use angle: ${gles_use_angle}")

# SubDirectories.
add_subdirectory(apps)
add_subdirectory(external)
add_subdirectory(base)
add_subdirectory(components)
add_subdirectory(guicomponents)
add_subdirectory(entities)
add_subdirectory(gui)
add_subdirectory(unittests)
add_subdirectory(desktop)







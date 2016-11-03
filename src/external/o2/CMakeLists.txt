cmake_minimum_required(VERSION 2.8.11)

if(POLICY CMP0048) # in CMake >= 3.0.0
    cmake_policy(SET CMP0048 OLD) # keep project() from clearing VERSION variables
endif(POLICY CMP0048)
set(VER_MAJOR "1")
set(VER_MINOR "0")
set(VER_PATCH "1")
set(API_VER_PATCH "0")
set(CPACK_PACKAGE_VERSION_MAJOR ${VER_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VER_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VER_PATCH})
set(PROJECT_VERSION ${VER_MAJOR}.${VER_MINOR}.${VER_PATCH})
set(API_VERSION ${VER_MAJOR}.${VER_MINOR}.${API_VER_PATCH})

project(o2)

set(o2_WITH_QT5 ON)

set(o2_LIB_SUFFIX "" CACHE STRING "Suffix for install 'lib' directory, e.g. 64 for lib64")

set(o2_SHOW_TRACE OFF)
if(NOT o2_SHOW_TRACE)
  add_definitions(-DQT_NO_DEBUG_OUTPUT=1)
endif()

set(o2_WITH_TWITTER ON)
set(o2_WITH_DROPBOX ON)
set(o2_WITH_GOOGLE ON)
set(o2_WITH_FACEBOOK ON)
set(o2_WITH_SKYDRIVE ON)
set(o2_WITH_FLICKR ON)
set(o2_WITH_HUBIC ON)
set(o2_WITH_SPOTIFY ON)
set(o2_WITH_SURVEYMONKEY ON)

set(o2_WITH_OAUTH1 OFF)
if(o2_WITH_TWITTER OR o2_WITH_DROPBOX OR o2_WITH_FLICKR)
  set(o2_WITH_OAUTH1 ON)
endif()

set(o2_BUILD_EXAMPLES OFF)

if(WIN32)
  add_definitions(-DO2_DLL_EXPORT)
endif()

add_subdirectory(src)

if(o2_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(o2_BUILD_EXAMPLES)




# Quote from cmake developer's commit message.

# This change adds support for Windows 10 Universal (Store) Applications.
#
# A Windows 10 Universal Application can be targeted by setting:
# CMAKE_SYSTEM_NAME=WindowsStore and CMAKE_SYSTEM_VERSION=10.0
# There are no WindowsPhone apps, universal apps target both phone and store.
# Specifying the CMAKE_SYSTEM_VERSION to be 10.0 will use the latest Windows
# 10 SDK installed. If you want to specify a specific SDK, you can make
# CMAKE_SYSTEM_VERSION be more specific (10.0.10240.0 for RTM for example).
#
#
# New Properties Added:
# VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION: Target Property.
# Specifies the minimum version of the OS that the project can target.
#
# VS_DESKTOP_EXTENSIONS_VERSION, VS_MOBILE_EXTENSIONS_VERSIONS,
# VS_IOT_EXTENSIONS_VERSION: Target property. When specifying these properties
# a reference to the version of the SDK specified will be added to the
# target allowing to target the extended functionality in a universal project.
# To match the version of the SDK being used, use
# CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION
#
# VS_IOT_STARTUP_TASK: Target property. Specifies that the target should be
# built as an IOT continuous background task.

#set(CMAKE_GENERATOR "Visual Studio 14 2015 Win64" CACHE INTERNAL "Name of generator.")

set(CMAKE_SYSTEM_NAME "WindowsStore" CACHE STRING "system name")
set(CMAKE_SYSTEM_VERSION "10.0" CACHE STRING "system version")

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Debug Release RelWithDebInfo MinSizeRel.")
set(ARCH "ARCH_WINRT" CACHE STRING "ARCH_LINUX, ARCH_WINDOWS, ARCH_ANDROID")


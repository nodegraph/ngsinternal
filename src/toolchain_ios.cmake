


# grab some environment variables
set(ARCH_SIM $ENV{ARCH_SIM})

# check that we got the environment variables
if (NOT ARCH_SIM)
    message(FATAL_ERROR "ARCH_SIM was not defined in shell!")
endif()


if (${ARCH_SIM} STREQUAL "device")
	SET (CMAKE_OSX_ARCHITECTURES "armv7" "arm64")
	set(ios_sdk "iphoneos")
	set(target "arm-apple-darwin")
	set(ios_arch "-arch armv7 -arch arm64")
	set(min_ios_sdk "-mios-version-min=8.0")
	execute_process(COMMAND xcrun -sdk ${ios_sdk} --show-sdk-path
	  OUTPUT_VARIABLE sysroot
	  OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND xcodebuild -sdk ${ios_sdk} -version SDKVersion
		OUTPUT_VARIABLE IOS_SDK_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif (${ARCH_SIM} STREQUAL "simulator")
	SET (CMAKE_OSX_ARCHITECTURES "i386" "x86_64")
	set(ios_sdk "iphonesimulator")
	#set(target "i386-apple-darwin")
	set(target "x86_64-apple-darwin")        
	set(ios_arch "-arch x86_64") # -arch i386  <- qt doesn't package these
	set(min_ios_sdk "-mios-version-min=8.0")
	execute_process(COMMAND xcrun -sdk ${ios_sdk} --show-sdk-path
	  OUTPUT_VARIABLE sysroot
	  OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND xcodebuild -sdk ${ios_sdk} -version SDKVersion
		OUTPUT_VARIABLE IOS_SDK_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()



# debugging
message("ARCH SIM IS: ${ARCH_SIM}")
message("ios sdk is: ${ios_sdk}")
message("ios arch is: ${ios_arch}")
message("sysroot is: ${sysroot}")
message("min ios sdk: ${min_ios_sdk}")

set(CMAKE_OSX_SYSROOT ${sysroot} CACHE PATH "" FORCE)
#set(CMAKE_OSX_DEPLOYMENT_TARGET 9.3)
exec_program(/usr/bin/xcode-select ARGS -print-path OUTPUT_VARIABLE CMAKE_XCODE_DEVELOPER_DIR)

set(CMAKE_MACOSX_BUNDLE YES)
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
set(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf")  #"dwarf-with-dsym"

set(CMAKE_SYSTEM_NAME Darwin CACHE STRING "" FORCE)
set(CMAKE_SYSTEM_VERSION ${IOS_SDK_VERSION} CACHE STRING "" FORCE)
set(UNIX TRUE CACHE BOOL "" FORCE)
set(APPLE TRUE CACHE BOOL "" FORCE)
set(IOS TRUE CACHE BOOL "" FORCE)

# We can't run ios apps from the build dir anyways so 
# we build using the install rpath.
set(CMAKE_BUILD_WITH_INSTALL_RPATH 1)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Debug Release RelWithDebInfo MinSizeRel.")
set(ARCH "ARCH_IOS" CACHE STRING "ARCH_LINUX, ARCH_WINDOWS, ARCH_ANDROID, ARCH_MACOS, ARCH_IOS")

# Compilers.

execute_process(COMMAND xcrun -sdk ${ios_sdk} -find cc
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output)
string(STRIP ${output} output)
set(CMAKE_C_COMPILER ${output} CACHE PATH "" FORCE)
message("c compiler is: ${CMAKE_C_COMPILER}")

execute_process(COMMAND xcrun -sdk ${ios_sdk} -find c++
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output)
string(STRIP ${output} output)
set(CMAKE_CXX_COMPILER ${output} CACHE PATH "" FORCE)
message("cxx compiler is: ${CMAKE_CXX_COMPILER}")

set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER} CACHE PATH "" FORCE)
message("asm compiler is: ${CMAKE_ASM_COMPILER}")

# Compiler flags.
set(CMAKE_C_COMPILER_ARG1 "-isysroot ${sysroot} ${ios_arch} ${min_ios_sdk}" CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER_ARG1 "-isysroot ${sysroot} ${ios_arch} ${min_ios_sdk}" CACHE STRING "" FORCE)
set(CMAKE_ASM_COMPILER_ARG1 "-isysroot ${sysroot} ${ios_arch} ${min_ios_sdk}" CACHE STRING "" FORCE)

# Default symbol export flags.
set(EXPORT_SYMBOLS_FLAG "-fvisibility=hidden ")

# Debug compiler flags.
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wl,-undefined,error ${EXPORT_SYMBOLS_FLAG} -fPIC -std=gnu++11" CACHE STRING "c++ compiler"  FORCE)
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g -Wl,-undefined,error ${EXPORT_SYMBOLS_FLAG} -fPIC" CACHE STRING "c compiler" FORCE)

# Release compiler flags.
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wl,-undefined,error ${EXPORT_SYMBOLS_FLAG} -fPIC -std=gnu++11" CACHE STRING "c++ compiler" FORCE)
set(CMAKE_C_FLAGS_RELEASE   "-O3 -Wl,-undefined,error ${EXPORT_SYMBOLS_FLAG} -fPIC" CACHE STRING "c compioler" FORCE)

# link flags
set(OUR_LINK_FLAGS " -L${sysroot}/usr/lib ")
set(CMAKE_EXE_LINKER_FLAGS "${OUR_LINK_FLAGS} " CACHE STRING "" FORCE) 
set(CMAKE_MODULE_LINKER_FLAGS "${OUR_LINK_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "${OUR_LINK_FLAGS}" CACHE STRING "" FORCE)

# other utils

execute_process(COMMAND xcrun -sdk ${ios_sdk} -find ld
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output)
string(STRIP ${output} output)
set(CMAKE_LINKER ${output} CACHE PATH "" FORCE)
message("ld is: ${CMAKE_LINKER}")

execute_process(COMMAND xcrun -sdk ${ios_sdk} -find ar
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output)
string(STRIP ${output} output)
set(CMAKE_AR ${output} CACHE PATH "" FORCE)
message("ar is: ${CMAKE_AR}")

execute_process(COMMAND xcrun -sdk ${ios_sdk} -find nm
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output)
string(STRIP ${output} output)
set(CMAKE_NM ${output} CACHE PATH "" FORCE)
message("ar is: ${CMAKE_NM}")

#execute_process(COMMAND xcrun -sdk ${ios_sdk} -find objcopy
#  RESULT_VARIABLE result
#  OUTPUT_VARIABLE output)
#string(STRIP ${output} output)
#set(CMAKE_OBJCOPY ${output} CACHE PATH "" FORCE)
#message("ar is: ${CMAKE_OBJCOPY}")

execute_process(COMMAND xcrun -sdk ${ios_sdk} -find llvm-objdump
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output)
string(STRIP ${output} output)
set(CMAKE_OBJDUMP ${output} CACHE PATH "" FORCE)
message("ar is: ${CMAKE_OBJDUMP}")

execute_process(COMMAND xcrun -sdk ${ios_sdk} -find ranlib
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output)
string(STRIP ${output} output)
set(CMAKE_RANLIB ${output} CACHE PATH "" FORCE)
message("ar is: ${CMAKE_RANLIB}")

execute_process(COMMAND xcrun -sdk ${ios_sdk} -find strip
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output)
string(STRIP ${output} output)
set(CMAKE_STRIP ${output} CACHE PATH "" FORCE)
message("ar is: ${CMAKE_STRIP}")

# where is the target environment located
SET(CMAKE_FIND_ROOT_PATH ${sysroot})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


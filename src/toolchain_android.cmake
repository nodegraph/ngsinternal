
# grab some environment variables
string(REPLACE "\\" "/" ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})
string(REPLACE "\\" "/" ANDROID_NDK_ROOT $ENV{ANDROID_NDK_ROOT})
string(REPLACE "\\" "/" ANDROID_TOOLCHAIN_ROOT $ENV{ANDROID_TOOLCHAIN_ROOT})
string(REPLACE "\\" "/" ANDROID_DEV_ROOT $ENV{ANDROID_DEV_ROOT})
set(ANDROID_DEVICE_ID $ENV{ANDROID_DEVICE_ID})
message("android device id is: $ENV{ANDROID_DEVICE_ID}")

# check that we got the environment variables
if (NOT ANDROID_SDK_ROOT)
    message(FATAL_ERROR "ANDROID_SDK_ROOT was not defined in shell!")
endif()
if (NOT ANDROID_NDK_ROOT)
    message(FATAL_ERROR "ANDROID_NDK_ROOT was not defined in shell!")
endif()
if (NOT ANDROID_TOOLCHAIN_ROOT)
    message(FATAL_ERROR "ANDROID_TOOLCHAIN_ROOT was not defined in shell!")
endif()
if (NOT ANDROID_TOOLCHAIN_ROOT)
    message(FATAL_ERROR "ANDROID_TOOLCHAIN_ROOT was not defined in shell!")
endif()

# setup some other custom cmake variables
set(ANDROID_TOOLCHAIN_BIN ${ANDROID_TOOLCHAIN_ROOT}/bin) 
set(ANDROID_TOOLCHAIN_OTHER ${ANDROID_TOOLCHAIN_ROOT}/arm-linux-androideabi/bin)
set(ANDROID_SYSTEM_ROOT ${ANDROID_TOOLCHAIN_ROOT}/sysroot)

set(gles_major_version 3 CACHE STRING "gles major version")
set(gles_minor_version 0 CACHE STRING "gles minor version")
set(gles_use_angle 0 CACHE STRING "whether to use angle .. 1 means yes, 0 means no")

# debugging
#message("android sdk root is: ${ANDROID_SDK_ROOT}")
#message("android ndk root is: ${ANDROID_NDK_ROOT}")
#message("android toolchain root is: ${ANDROID_TOOLCHAIN_ROOT}")
#message("android dev root is: ${ANDROID_DEV_ROOT}")

# the name of the target operating system
set(CMAKE_SYSTEM_NAME Generic) # this make the cxx compiler test not give this: --out-implib: unknown option

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Debug Release RelWithDebInfo MinSizeRel.")
set(ARCH "ARCH_ANDROID" CACHE STRING "ARCH_LINUX, ARCH_WINDOWS, DESKTOP_NACL, DEKSTOP_PNACL, ARCH_ANDROID")

# which compilers to use for C and C++
set(CMAKE_C_COMPILER ${ANDROID_TOOLCHAIN_BIN}/arm-linux-androideabi-gcc.exe)
set(CMAKE_C_COMPILER_ARG1 "--sysroot=${ANDROID_SYSTEM_ROOT}")
set(CMAKE_CXX_COMPILER ${ANDROID_TOOLCHAIN_BIN}/arm-linux-androideabi-g++.exe)
set(CMAKE_CXX_COMPILER_ARG1 "--sysroot=${ANDROID_SYSTEM_ROOT}")
set(CMAKE_ASM_COMPILER "${ANDROID_TOOLCHAIN_BIN}/arm-linux-androideabi-gcc.exe" CACHE PATH "assembler" )
set(CMAKE_ASM_COMPILER_ARG1 "--sysroot=${ANDROID_SYSTEM_ROOT}")

# Default symbol export flags.
set(EXPORT_SYMBOLS_FLAG "-Wl,--no-allow-shlib-undefined -fvisibility=hidden ")

# Debug compiler flags.
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wl,-z,defs -Wl,--no-undefined ${EXPORT_SYMBOLS_FLAG} -fPIC -std=gnu++11" CACHE STRING "c++ compiler") #Debug options." FORCE)
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g -Wl,-z,defs -Wl,--no-undefined ${EXPORT_SYMBOLS_FLAG} -fPIC" CACHE STRING "c compiler") #Debug options." FORCE)

# Release compiler flags.
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wl,-z,defs -Wl,--no-undefined ${EXPORT_SYMBOLS_FLAG} -fPIC -std=gnu++11" CACHE STRING "c++ compiler")
set(CMAKE_C_FLAGS_RELEASE   "-O3 -Wl,-z,defs -Wl,--no-undefined ${EXPORT_SYMBOLS_FLAG} -fPIC" CACHE STRING "c compioler")

# link flags
# picks up gnustl_shared and GLESv3 libs
set(OUR_LINK_FLAGS "-L${ANDROID_NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a -L${ANDROID_SYSTEM_ROOT}/usr/lib ")
set(CMAKE_EXE_LINKER_FLAGS "${OUR_LINK_FLAGS}") 
set(CMAKE_MODULE_LINKER_FLAGS "${OUR_LINK_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${OUR_LINK_FLAGS}")

# other utils
set(CMAKE_LINKER ${ANDROID_TOOLCHAIN_OTHER}/ld CACHE PATH "linker")
set(CMAKE_AR ${ANDROID_TOOLCHAIN_OTHER}/ar CACHE PATH "archive")
set(CMAKE_NM ${ANDROID_TOOLCHAIN_OTHER}/nm CACHE PATH "nm")
set(CMAKE_OBJCOPY ${ANDROID_TOOLCHAIN_OTHER}/objcopy CACHE PATH "objcopy")
set(CMAKE_OBJDUMP ${ANDROID_TOOLCHAIN_OTHER}/objdump CACHE PATH "objdump")
set(CMAKE_RANLIB ${ANDROID_TOOLCHAIN_OTHER}/ranlib CACHE PATH "ranlib")
set(CMAKE_STRIP ${ANDROID_TOOLCHAIN_OTHER}/strip CACHE PATH "strip")

# where is the target environment located
SET(CMAKE_FIND_ROOT_PATH ${ANDROID_SYSTEM_ROOT})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


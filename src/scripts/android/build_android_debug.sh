#!/bin/bash

# Jenkins Pipeline Definition.
#node('windows') {
#    bat "D:\\src\\ngsinternal\\src\\scripts\\msys2\\launch_msys2.bat ARCH_ANDROID x32 D:\\src\\ngsinternal\\src\\scripts\\android\\build_android_debug.sh"
#}

echo "ANDROID DEBUG";
export ARCH=ARCH_ANDROID;
export ARCH_BITS=x32;
source /d/src/ngsinternal/src/scripts/msys2/setup_env.sh;

# recreate our previous root build root and cd into it 
CMAKE_BUILD_ROOT=/d/android_debug
rm -fr $CMAKE_BUILD_ROOT
mkdir -p $CMAKE_BUILD_ROOT
cd $CMAKE_BUILD_ROOT

# create our ninja files
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=/d/src/ngsinternal/src/toolchain_android.cmake -DCMAKE_BUILD_TYPE=Debug /d/src/ngsinternal/src;

# build
ninja install;


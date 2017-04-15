#!/bin/bash

# Jenkins Pipeline Definition.
#node('windows') {
#      withCredentials([[$class: 'UsernamePasswordMultiBinding', credentialsId: '1929422f-fa6e-462d-84be-8f74cbeb1809',
#                            usernameVariable: 'USERNAME', passwordVariable: 'PASSWORD']]) {
#                //available as an env variable, but will be masked if you try to print it out any which way
#                bat "D:\\src\\ngsinternal\\src\\scripts\\msys2\\launch_msys2.bat ARCH_WINDOWS x64 D:\\src\\ngsinternal\\src\\scripts\\msys2\\build_windows_debug.sh"
#            }
#}

# Jenkins Pipeline Definition.
#node('windows') {
#    bat "D:\\src\\ngsinternal\\src\\scripts\\msys2\\launch_msys2.bat ARCH_WINDOWS x64 D:\\src\\ngsinternal\\src\\scripts\\msys2\\build_windows_debug.sh"
#}

echo "WINDOWS DEBUG";
export ARCH=ARCH_WINDOWS;
export ARCH_BITS=x64;
source /d/src/ngsinternal/src/scripts/msys2/setup_env.sh;

# recreate our previous root build root and cd into it 
CMAKE_BUILD_ROOT=/d/wd64
rm -fr $CMAKE_BUILD_ROOT
mkdir -p $CMAKE_BUILD_ROOT
cd $CMAKE_BUILD_ROOT

# create our ninja files
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug /d/src/ngsinternal/src;

# build
ninja install;

# youmacro installers and repos $CMAKE_BUILD_ROOT
youmacro_installer_windows.sh package $CMAKE_BUILD_ROOT
youmacro_installer_windows.sh create_repo $CMAKE_BUILD_ROOT
youmacro_installer_windows.sh create_installer $CMAKE_BUILD_ROOT


#!/bin/bash

# Jenkins Pipeline Definition.
#node('windows') {
#    bat "D:\\src\\ngsinternal\\src\\scripts\\msys2\\launch_msys2.bat ARCH_WINDOWS x64 D:\\src\\ngsinternal\\src\\scripts\\msys2\\build_window_debug_x64.sh"
#}

echo "WINDOWS DEBUG";
export ARCH=ARCH_WINDOWS;
export ARCH_BITS=x64;
source /d/src/ngsinternal/src/scripts/msys2/setup_env.sh;
rm -fr /d/wd64;
mkdir /d/wd64;
cd /d/wd64;
git clone git@gitlab.com:nodegraph/ngsinternal.git;
mkdir build;
cd build;
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ../ngsinternal/src;
ninja install;

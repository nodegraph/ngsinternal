#!/bin/bash

# Jenkins Pipeline Definition.
#node('windows') {
#    bat "D:\\src\\ngsinternal\\src\\scripts\\msys2\\launch_msys2.bat ARCH_WINDOWS x64 D:\\src\\ngsinternal\\src\\scripts\\msys2\\build_windows_release.sh"
#}

echo "WINDOWS RELEASE";
export ARCH=ARCH_WINDOWS;
export ARCH_BITS=x64;
source /d/src/ngsinternal/src/scripts/msys2/setup_env.sh;
rm -fr /d/wr64;
mkdir /d/wr64;
cd /d/wr64;
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release /d/src/ngsinternal/src;
ninja install;

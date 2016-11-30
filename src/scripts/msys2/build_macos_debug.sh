#!/bin/bash

# Jenkins Pipeline Definition.
#node('macos') {
#    sh "source /Users/raindrop/src/ngsinternal/src/scripts/msys2/build_macos_debug.sh"
#}

echo "MACOS DEBUG"
#export ARCH=ARCH_MACOS
#export ARCH_BITS=x64
echo ${PATH}
which cmake
source /Users/raindrop/src/ngsinternal/src/scripts/msys2/setup_env.sh
echo ${PATH}
which cmake
cd /Users/raindrop/src/ngsinternal
git pull origin master
cd /Users/raindrop/dev/macos
rm -fr macos_debug_jenkins
mkdir macos_debug_jenkins
cd macos_debug_jenkins
mkdir build
cd build
cmake -G "Eclipse CDT4 - Ninja" -DARCH=ARCH_MACOS -DCMAKE_BUILD_TYPE=Debug ~/src/ngsinternal/src
ninja install



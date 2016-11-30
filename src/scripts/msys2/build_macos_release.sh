#!/bin/bash

# Jenkins Pipeline Definition.
#node('macos') {
#    sh "source /Users/raindrop/src/ngsinternal/src/scripts/msys2/build_macos_release.sh"
#}

echo "MACOS RELEASE"
#export ARCH=ARCH_MACOS
#export ARCH_BITS=x64
cd /Users/raindrop/src/ngsinternal
git pull
cd /Users/raindrop/dev/macos
rm -fr macos_release_jenkins
mkdir macos_release_jenkins
cd macos_release_jenkins
mkdir build
cd build
cmake -G "Eclipse CDT4 - Ninja" -DARCH=ARCH_MACOS -DCMAKE_BUILD_TYPE=Release ../ngsinternal/src
ninja install



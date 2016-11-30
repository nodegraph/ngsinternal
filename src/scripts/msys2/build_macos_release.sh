#!/bin/bash

# Jenkins Pipeline Definition.
#node('macos') {
#    sh "source /Users/raindrop/src/ngsinternal/src/scripts/msys2/build_macos_release.sh"
#}

echo "MACOS RELEASE"
export ARCH=ARCH_MACOS
export ARCH_BITS=x64
source /Users/raindrop/src/ngsinternal/src/scripts/msys2/setup_env.sh
cd /Users/raindrop/dev/macos
rm -fr macos_release_jenkins
mkdir macos_release_jenkins
cd macos_release_jenkins
git clone git@gitlab.com:nodegraph/ngsinternal.git;
mkdir build;
cd build;
cmake -G "Eclipse CDT4 - Unix Makefiles" -DARCH=ARCH_MACOS -DCMAKE_BUILD_TYPE=Release ../ngsinternal/src
make install -j8



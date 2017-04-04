#!/bin/bash

# Jenkins Pipeline Definition.
#node('macos') {
#    sh "source /Users/raindrop/src/ngsinternal/src/scripts/macos/build_macos_release.sh"
#}

whoami

echo "MACOS RELEASE"
export ARCH=ARCH_MACOS
export ARCH_BITS=x64
source ~/src/ngsinternal/src/scripts/macos/setup_env.sh

# recreate our previous root build root and cd into it 
CMAKE_BUILD_ROOT=~raindrop/dev/macos/macos_release_jenkins
sudo rm -fr $CMAKE_BUILD_ROOT
mkdir -p $CMAKE_BUILD_ROOT
cd $CMAKE_BUILD_ROOT

# create our ninja files
cmake -G "Eclipse CDT4 - Ninja" -DARCH=ARCH_MACOS -DCMAKE_BUILD_TYPE=Release ~/src/ngsinternal/src

# build
ninja install
ninja fill_robodownloader

# robodownloader installers and repos
robodownloader_installer_macos.sh package $CMAKE_BUILD_ROOT
robodownloader_installer_macos.sh create_repo $CMAKE_BUILD_ROOT
robodownloader_installer_macos.sh create_installer $CMAKE_BUILD_ROOT



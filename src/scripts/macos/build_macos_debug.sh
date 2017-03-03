#!/bin/bash

# Jenkins Pipeline Definition.
#node('macos') {
#    sh "source /Users/raindrop/src/ngsinternal/src/scripts/macos/build_macos_debug.sh"
#}

echo "MACOS DEBUG"
export ARCH=ARCH_MACOS
export ARCH_BITS=x64
source ~/src/ngsinternal/src/scripts/macos/setup_env.sh
cd ~/dev/macos
sudo rm -fr macos_debug_jenkins
mkdir macos_debug_jenkins
cd macos_debug_jenkins
cmake -G "Eclipse CDT4 - Ninja" -DARCH=ARCH_MACOS -DCMAKE_BUILD_TYPE=Debug ~/src/ngsinternal/src

# build
ninja install
ninja fill_smashbrowse
ninja fill_smashdownloader

# smashbrowse installers and repos
smashbrowse_installer_macos.sh package debug
smashbrowse_installer_macos.sh create_repo debug
smashbrowse_installer_macos.sh create_installer debug

# smashdownloader installers and repos
smashdownloader_installer_macos.sh package debug
smashdownloader_installer_macos.sh create_repo debug
smashdownloader_installer_macos.sh create_installer debug
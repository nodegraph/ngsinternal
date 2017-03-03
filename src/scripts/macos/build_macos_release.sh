#!/bin/bash

# Jenkins Pipeline Definition.
#node('macos') {
#    sh "source /Users/raindrop/src/ngsinternal/src/scripts/macos/build_macos_release.sh"
#}

echo "MACOS RELEASE"
export ARCH=ARCH_MACOS
export ARCH_BITS=x64
source ~/src/ngsinternal/src/scripts/macos/setup_env.sh
cd ~/dev/macos
sudo rm -fr macos_release_jenkins
mkdir macos_release_jenkins
cd macos_release_jenkins
cmake -G "Eclipse CDT4 - Ninja" -DARCH=ARCH_MACOS -DCMAKE_BUILD_TYPE=Release ~/src/ngsinternal/src

# build
ninja install
ninja fill_smashbrowse
ninja fill_smashdownloader

# smashbrowse installers and repos
smashbrowse_installer_macos.sh package release
smashbrowse_installer_macos.sh create_repo release
smashbrowse_installer_macos.sh create_installer release

# smashdownloader installers and repos
smashdownloader_installer_macos.sh package release
smashdownloader_installer_macos.sh create_repo release
smashdownloader_installer_macos.sh create_installer release



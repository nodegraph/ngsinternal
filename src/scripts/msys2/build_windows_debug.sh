#!/bin/bash

# Jenkins Pipeline Definition.
#node('windows') {
#    bat "D:\\src\\ngsinternal\\src\\scripts\\msys2\\launch_msys2.bat ARCH_WINDOWS x64 D:\\src\\ngsinternal\\src\\scripts\\msys2\\build_windows_debug.sh"
#}

echo "WINDOWS DEBUG";
export ARCH=ARCH_WINDOWS;
export ARCH_BITS=x64;
source /d/src/ngsinternal/src/scripts/msys2/setup_env.sh;
rm -fr /d/wd64;
mkdir /d/wd64;
cd /d/wd64;
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug /d/src/ngsinternal/src;

# build
ninja install;

# smashbrowse installers and repos
smashbrowse_installer_windows.sh package debug
smashbrowse_installer_windows.sh create_repo debug
smashbrowse_installer_windows.sh create_installer debug

# smashbrowse installers and repos
smashdownloader_installer_windows.sh package debug
smashdownloader_installer_windows.sh create_repo debug
smashdownloader_installer_windows.sh create_installer debug
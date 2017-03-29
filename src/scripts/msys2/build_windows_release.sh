#!/bin/bash

# Jenkins Pipeline Definition.
#node('windows') {
#    bat "D:\\src\\ngsinternal\\src\\scripts\\msys2\\launch_msys2.bat ARCH_WINDOWS x64 D:\\src\\ngsinternal\\src\\scripts\\msys2\\build_windows_release.sh"
#}

echo "WINDOWS RELEASE";
export ARCH=ARCH_WINDOWS;
export ARCH_BITS=x64;
source /d/src/ngsinternal/src/scripts/msys2/setup_env.sh;

# recreate our previous root build root and cd into it 
CMAKE_BUILD_ROOT=/d/wr64
rm -fr $CMAKE_BUILD_ROOT
mkdir -p $CMAKE_BUILD_ROOT
cd $CMAKE_BUILD_ROOT

# create our ninja files
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release /d/src/ngsinternal/src;

# build
ninja install;

# smashbrowse installers and repos
smashbrowse_installer_windows.sh package release $CMAKE_BUILD_ROOT
smashbrowse_installer_windows.sh create_repo release $CMAKE_BUILD_ROOT
smashbrowse_installer_windows.sh create_installer release $CMAKE_BUILD_ROOT


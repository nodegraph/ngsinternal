#!/bin/bash

# Jenkins Pipeline Definition.
#node('macos') {
#      withCredentials([[$class: 'UsernamePasswordMultiBinding', credentialsId: 'adf8e49d-3391-4d48-947b-2b8d6fcc1170',
#                            usernameVariable: 'USERNAME', passwordVariable: 'PASSWORD']]) {
#                //available as an env variable, but will be masked if you try to print it out any which way
#                sh "source /Users/raindrop/src/ngsinternal/src/scripts/macos/build_macos_debug.sh" $PASSWORD
#            }
#}

whoami

# Debugging to see if username and passwords come through properly from the credentials.
# echo ${PASSWORD} | sed -e 's/\(.\)/\1 /g'
# echo ${USERNAME} | sed -e 's/\(.\)/\1 /g'

# Unlock our keychain so that we can codesign.
security unlock-keychain -p $PASSWORD $HOME/Library/Keychains/login.keychain-db

# Setup our build env.
echo "MACOS DEBUG"
export ARCH=ARCH_MACOS
export ARCH_BITS=x64
source ~/src/ngsinternal/src/scripts/macos/setup_env.sh

# recreate our previous root build root and cd into it 
CMAKE_BUILD_ROOT=~raindrop/dev/macos/macos_debug_jenkins
sudo rm -fr $CMAKE_BUILD_ROOT
mkdir -p $CMAKE_BUILD_ROOT
cd $CMAKE_BUILD_ROOT

# create our ninja files
cmake -G "Eclipse CDT4 - Ninja" -DARCH=ARCH_MACOS -DCMAKE_BUILD_TYPE=Debug ~/src/ngsinternal/src

# build
ninja install
ninja fill_robodownloader

# Get permission to use codesign certificates.
#security set-key-partition-list -S apple: -k $1 -D raindrop -t private
security set-key-partition-list -s -k $1 -D raindrop -t private

# robodownloader installers and repos
robodownloader_installer_macos.sh package $CMAKE_BUILD_ROOT
robodownloader_installer_macos.sh create_repo $CMAKE_BUILD_ROOT
robodownloader_installer_macos.sh create_installer $CMAKE_BUILD_ROOT


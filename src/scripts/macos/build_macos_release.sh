#!/bin/bash

# Jenkins Pipeline Definition.
#node('macos') {
#      withCredentials([[$class: 'UsernamePasswordMultiBinding', credentialsId: '1929422f-fa6e-462d-84be-8f74cbeb1809',
#                            usernameVariable: 'USERNAME', passwordVariable: 'PASSWORD']]) {
#                //available as an env variable, but will be masked if you try to print it out any which way
#                sh 'echo $PASSWORD'
#                echo "${env.USERNAME}"
#                sh "source /Users/raindrop/src/ngsinternal/src/scripts/macos/build_macos_release.sh" $PASSWORD
#            }
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

# Get permission to use codesign certificates.
security unlock-keychain -p $1 "/Users/raindrop/Library/Keychains/login.keychain-db

#security set-key-partition-list -S apple: -k $1 -D raindrop -t private
#security set-key-partition-list -s -k $1 -D raindrop -t private

# robodownloader installers and repos
robodownloader_installer_macos.sh package $CMAKE_BUILD_ROOT
robodownloader_installer_macos.sh create_repo $CMAKE_BUILD_ROOT
robodownloader_installer_macos.sh create_installer $CMAKE_BUILD_ROOT



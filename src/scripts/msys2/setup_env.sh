#!/bin/bash
# --------------------------------------------------------------------
# These are the main env variables that need to be configured.
# The other variables below depend on where you installed other tools
# like eclipse, ninja etc.
#



# setup the software locations

# SRC_ROOT points to the location where you checked out the repos,
# like ngsinternal. This file is expected to be in that repo.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
export SRC_ROOT=`readlink -f ${SCRIPT_DIR}/../../../..`
echo "setting SRC_ROOT as: ${SRC_ROOT}"

# PLATFORM_ROOT points to the location where you install applications
# on your system, like eclipse. You can search for uses of
# PLATOFRM_ROOT to see what types of apps we have installed.
# We currently expect ngsexternal to be checkout in ${PLATFORM_ROOT}/srcdeps.
export PLATFORM_ROOT=/d/installs

# --------------------------------------------------------------------

# FileChecksumVerifier
PATH=${PLATFORM_ROOT}/windowsunpack/FileChecksumIntegrityVerifier:${PATH}

# CMake
PATH="/c/Program Files (x86)/CMake/bin":${PATH}

# Path for wix.
PATH="/c/Program Files (x86)/WiX Toolset v3.10/bin":${PATH}

# Path for xxd, which is part of vim for windows
PATH="/c/Program Files (x86)/Vim/vim74":${PATH}

# Jom build tool
PATH=${PLATFORM_ROOT}/windowsunpack/jom_1_1_0:${PATH}

# Ninja build tool
PATH=${PLATFORM_ROOT}/windowsunpack/ninja-win:${PATH}

# Eclipse
PATH=${PLATFORM_ROOT}/windowsunpack/eclipse:${PATH}

# Ant
PATH=${PLATFORM_ROOT}/windowsunpack/apache-ant-1.9.6/bin:${PATH}

# Android platform tools
PATH=/d/installs/android/android-sdk/platform-tools:${PATH}

# Git
PATH="/c/Program Files/Git/bin":${PATH}

# GitLab CI
PATH=/d/installs/windowsunpack/gitlab_ci:${PATH}

# Node installed modules, top level npm dir..
PATH=/C/Users/Shingo/AppData/Roaming/npm:${PATH}

# Path for Mosquitto
PATH="/c/Program Files (x86)/mosquitto":${PATH}



# Java home
# This is the 32 bit version of the jdk which was installed by Qt
# and is need by androiddeployqt.
# Note that a 64bit version of jdk8 is on the path also.
# The 64bit version is neede for eclipse.
# Putting the 32bit version of the jdk7 first messes up eclipse startup.
export JAVA_HOME="/c/Program Files (x86)/Java/jdk1.7.0_55"
#PATH="/c/Program Files (x86)/Java/jdk1.7.0_55/bin":${PATH}
PATH="/c/Program Files/Java/jdk1.8.0_74/bin":${PATH}

# Some aliases.
alias cd_src="cd ${SRC_ROOT}"


# Our scripts.
PATH=${SRC_ROOT}/ngsinternal/src/apps/smashbrowse:${PATH}
PATH=${SRC_ROOT}/ngsinternal/src/scripts/msys2:${PATH}

echo setting up for ${ARCH} ${ARCH_BITS}

# ARCH specific paths.
if [ ${ARCH} == "ARCH_WINDOWS" ]; then 
  echo
elif [ ${ARCH} == "ARCH_MACOS" ]; then
  echo
elif [ ${ARCH} == "ARCH_WINRT" ]; then
  echo
elif [ ${ARCH} == "ARCH_ANDROID" ]; then
  
  # setup android variables
  export ANDROID_SDK_ROOT=/d/installs/android/android-sdk
  export ANDROID_NDK_ROOT=/d/installs/android/android-ndk-r10e
  export ANDROID_TOOLCHAIN_ROOT=/d/installs/android/android_toolchain
  
  # check our android variables
  echo "           sdk root: $ANDROID_SDK_ROOT"
  echo "           ndk root: $ANDROID_NDK_ROOT"
  echo "     toolchain root: $ANDROID_TOOLCHAIN_ROOT"
  # check our device id
  echo "  android device id: $ANDROID_DEVICE_ID"

else
  echo unknown ARCH specified
fi

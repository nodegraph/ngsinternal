#!/bin/bash

# --------------------------------------------------------------------
# These are the main env variables that need to be configured.
# The other variables below depend on where you installed other tools
# like eclipse, ninja etc.

# setup the software locations

# SRC_ROOT points to the location where you checked out the repos,
# like ngsinternal. This file is expected to be in that repo.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo script dir is: ${SCRIPT_DIR}

export SRC_ROOT=$(cd "$(dirname "${SCRIPT_DIR}/../../../../..")"; pwd)
echo SRC_ROOT is: ${SRC_ROOT}

# PLATFORM_ROOT points to the location where you install applications
# on your system, like eclipse. You can search for uses of
# PLATOFRM_ROOT to see what types of apps we have installed.
# We currently expect ngsexternal to be checkout in ${PLATFORM_ROOT}/srcdeps.
export PLATFORM_ROOT=/Users/raindrop/installs

# --------------------------------------------------------------------

# CMake
PATH=${PLATFORM_ROOT}/macosunpacks/CMake.app/Contents/bin:${PATH}

# Ninja build tool
PATH=${PLATFORM_ROOT}/macosunpacks/ninja:${PATH}

# Eclipse
PATH=${PLATFORM_ROOT}/macosunpacks/Eclipse.app/Contents/MacOS:${PATH}

# Ant
PATH=${PLATFORM_ROOT}/macosunpacks/apache-ant-1.10.1/bin:${PATH}

# GitLab CI
PATH=${PLATFORM_ROOT}/macosunpacks/gitlab_ci:${PATH}

# Java home
# This is the 32 bit version of the jdk which was installed by Qt
# and is need by androiddeployqt.
# Note that a 64bit version of jdk8 is on the path also.
# The 64bit version is neede for eclipse.
# Putting the 32bit version of the jdk7 first messes up eclipse startup.
export JAVA_HOME="/c/Program Files (x86)/Java/jdk1.7.0_55"
#PATH="/c/Program Files (x86)/Java/jdk1.7.0_55/bin":${PATH}

# SourceTree
PATH=/Users/raindrop/installs/macosunpacks/SourceTree.app/Contents/MacOS:${PATH}

# Node and typesript
PATH=/Users/raindrop/installs/macosunpacks/node-v7.2.0-darwin-x64/bin:${PATH}

# Qt Installer Framework
PATH=/Users/raindrop/installs/macos/QtIFW2.0.3/bin:${PATH}

# Our scripts.
PATH=${SCRIPT_DIR}:${PATH}
PATH=${SRC_ROOT}/ngsinternal/src/apps/robodownloader:${PATH}

# Some aliases.
alias cd_src="cd ${SRC_ROOT}"


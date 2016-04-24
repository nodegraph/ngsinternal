

# --------------------------------------------------------------------
# These are the main env variables that need to be configured.
# The other variables below depend on where use installed other tools
# like eclipse, ninja etc.

# setup android variables
export ANDROID_SDK_ROOT=/d/installs/android/android-sdk
export ANDROID_NDK_ROOT=/d/installs/android/android-ndk-r10e
export ANDROID_TOOLCHAIN_ROOT=/d/installs/android/android_toolchain

# setup the software locations

# SRC_ROOT points to the location where you checked out the repos,
# like ngsinternal, ngsexternal and ngsrelease.
export SRC_ROOT=/d/src

# PLATFORM_ROOT points to the location where you install applications
# on your system, like eclipse. You can search for uses of
# PLATOFRM_ROOT to see what types of apps we have installed.
export PLATFORM_ROOT=/d/installs

# --------------------------------------------------------------------


# get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# FileChecksumVerifier
PATH=${PLATFORM_ROOT}/windowsunpack/FileChecksumIntegrityVerifier:${PATH}

# CMake
PATH="/c/Program Files (x86)/CMake/bin":${PATH}

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

# GitLab CI
PATH=/d/installs/windowsunpack/gitlab_ci:${PATH}

# Some aliases.
alias cd_src="cd ${SRC_ROOT}"

# ARCH specific paths.
if [ ${ARCH} == "ARCH_WINDOWS" ]; then 
	echo setting up for ARCH_WINDOWS
	
	# Qt dir.
	#PATH=/c/Qt/Qt5.5.1/5.5/msvc2013_64/bin:${PATH}
    #PATH=/d/installs/windows/Qt5.5.1/5.5/msvc2013_64/bin:${PATH}
    PATH=/d/installs/windows/Qt5.6.0/5.6/msvc2015_64/bin:${PATH}
	
	# Dll search dirs.
	PATH=${SRC_ROOT}/ngsexternal/vs2013/glew-1.13.0/lib/debugMX:${PATH}
	PATH=${SRC_ROOT}/ngsexternal/vs2013/glew-1.13.0/lib/releaseMX:${PATH}
	
elif [ ${ARCH} == "ARCH_WINRT" ]; then
	echo setting up for ARCH_WINRT
	
	# Qt dir.
	#PATH=/d/installs/winrt/Qt5.5.1/5.5/winrt_x64/bin:${PATH}
    PATH=/d/installs/winrt/Qt5.6.0/5.6/winrt_x64_msvc2015/bin:${PATH}

	
elif [ ${ARCH} == "ARCH_WINRT_ARM" ]; then
	echo setting up for ARCH_WINRT_ARM
	
	# Qt dir.
	PATH=/d/installs/winrt/Qt5.5.1/5.5/winphone_arm/bin:${PATH}
	
elif [ ${ARCH} == "ARCH_WINRT_X86" ]; then
	echo setting up for ARCH_WINRT_X86
	
	# Qt dir.
	PATH=/d/installs/winrt/Qt5.5.1/5.5/winphone_x86/bin:${PATH}
	
elif [ ${ARCH} == "ARCH_ANDROID" ]; then
	echo setting up for ARCH_ANDROID
	
	# Qt dir.
	#PATH=/d/installs/android/Qt5.5.1/5.5/android_armv7/bin:${PATH}
    PATH=/D/installs/android/Qt5.6.0/5.6/android_armv7/bin:${PATH}
	
	# check our android variables
	echo "           sdk root: $ANDROID_SDK_ROOT"
	echo "           ndk root: $ANDROID_NDK_ROOT"
	echo "     toolchain root: $ANDROID_TOOLCHAIN_ROOT"
	# check our device id
	echo "  android device id: $ANDROID_DEVICE_ID"

else
	echo unknown ARCH specified
fi

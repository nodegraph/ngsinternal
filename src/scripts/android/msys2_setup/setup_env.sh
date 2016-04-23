
# get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# setup android variables
export ANDROID_SDK_ROOT=/d/installs/android/android-sdk
export ANDROID_NDK_ROOT=/d/installs/android/android-ndk-r10e
export ANDROID_TOOLCHAIN_ROOT=/d/installs/android/android_toolchain

# setup the dev roots
export ANDROID_DEV_ROOT=/d/devandroid
export DEV_ROOT=/c/Users/shing/Documents

# FileChecksumVerifier
PATH=${DEV_ROOT}/apps/FileChecksumIntegrityVerifier:${PATH}

# Path for xxd, which is part of vim for windows
PATH="/c/Program Files (x86)/Vim/vim74":${PATH}

# Jom build tool
PATH=${DEV_ROOT}/apps/jom_1_1_0:${PATH}

# Ninja build tool
PATH=${DEV_ROOT}/apps/ninja-win:${PATH}

# Eclipse
PATH=${DEV_ROOT}/apps/eclipse:${PATH}

# Ant
PATH=/c/Users/shing/Documents/apps/apache-ant-1.9.6/bin:${PATH}

# Android platform tools
PATH=/d/installs/android/android-sdk/platform-tools:${PATH}

# Some aliases.
alias cd_dev="cd ${DEV_ROOT}"
alias cd_adev="cd ${ANDROID_DEV_ROOT}"
alias cd_src="cd ${ANDROID_DEV_ROOT}/nodegraphsoftware"

# ARCH specific paths.
if [ ${ARCH} == "ARCH_WINDOWS" ]; then 
	echo setting up for ARCH_WINDOWS
	
	# Qt dir.
	#PATH=/c/Qt/Qt5.5.1/5.5/msvc2013_64/bin:${PATH}
    #PATH=/d/installs/windows/Qt5.5.1/5.5/msvc2013_64/bin:${PATH}
    PATH=/d/installs/windows/Qt5.6.0/5.6/msvc2015_64/bin:${PATH}
	
	# Dll search dirs.
	PATH=${DEV_ROOT}/nodegraphexternal/vs2013/glew-1.13.0/lib/debugMX:${PATH}
	PATH=${DEV_ROOT}/nodegraphexternal/vs2013/glew-1.13.0/lib/releaseMX:${PATH}
	
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
	# check our dev areas
	echo "           dev root: $DEV_ROOT"
	echo "   android dev root: $ANDROID_DEV_ROOT"
	# check our device id
	echo "  android device id: $ANDROID_DEVICE_ID"

else
	echo unknown ARCH specified
fi

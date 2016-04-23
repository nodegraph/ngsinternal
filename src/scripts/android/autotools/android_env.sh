
echo "android_env.sh is setting up environemnt variables"

# The following should already be defined.
#echo "the android sdk root is: ${ANDROID_SDK_ROOT}"
#echo "the android ndk root is: ${ANDROID_NDK_ROOT}"
#echo "the android toolchain root is: ${ANDROID_TOOLCHAIN_ROOT}"

export ANDROID_API_LEVEL=android-21
export ANDROID_TARGET_ARCH=arch-arm
export ANDROID_SYS_ROOT=${ANDROID_TOOLCHAIN_ROOT}/android_toolchain/sysroot

_gpp="arm-linux-androideabi-g++ --sysroot=${ANDROID_SYS_ROOT} "
_gcc="arm-linux-androideabi-gcc --sysroot=${ANDROID_SYS_ROOT} "
_ar="arm-linux-androideabi-ar --sysroot=${ANDROID_SYS_ROOT} "
_ranlib="arm-linux-androideabi-ranlib --sysroot=${ANDROID_SYS_ROOT} "
_ld="arm-linux-androideabi-ld --sysroot=${ANDROID_SYS_ROOT} "
_nm="nm"
_objdump="objdump"
_objcopy="objdump"
_strip="strip"
_as="as"


# Setup aliases.
alias g++="${_gpp}"
alias cxx="${_gpp}"
alias c++="${_gpp}"
alias gcc="${_gcc}"
alias scc="${_gcc}"
alias ar="${_ar}"
alias ranlib="${_ranlib}"
alias ld="${_ld}"
alias nm="${_nm}"
alias objdump="${_objdump}"
alias objcopy="${_objcopy}"
alias strip="${_strip}"
alias as="${_as}"

# Setup env vars.
# The ldflags picks up libgnustl_shared.so for armeabi-v7a.
export CXX="${_gpp}"
export CC="${_gcc}"
export LD="${_ld}"
export LDFLAGS=" -L${ANDROID_NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a"
export CFLAGS=""
export CXXFLAGS="" 

# Setup path to pick up android toolchain.
export PATH=${ANDROID_TOOLCHAIN_ROOT}/bin:${PATH}

# Setup path to pick up other utils.
export PATH=${ANDROID_TOOLCHAIN_ROOT}/arm-linux-androideabi/bin:${PATH}

# Note when running configure scripts to build makefiles for android cross compile,
# you may need to specify the following:
#   --host=arm

# For example configure libpng as follows: (currenly not used. it's just for testing our compile setup.)
#./configure --prefix=/d/devandroid/local/libpng-1.6.21 --host=arm


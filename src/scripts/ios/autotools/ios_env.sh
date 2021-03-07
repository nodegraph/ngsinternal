#!/bin/bash

OPT_FLAGS="-O3"

setup_build_variables() {
    export CC="$(xcrun -sdk ${ios_sdk} -find cc) -isysroot ${sysroot} ${ios_arch} ${min_ios_sdk} "
    export CXX="$(xcrun -sdk ${ios_sdk} -find c++) -isysroot ${sysroot} ${ios_arch} ${min_ios_sdk} "
    
    export AR="$(xcrun -sdk ${ios_sdk} -find ar) "
    export LD="$(xcrun -sdk ${ios_sdk} -find ld) -isysroot ${sysroot} ${ios_arch} ${min_ios_sdk} "
    export LIBTOOL="$(xcrun -sdk ${ios_sdk} -find libtool) -isysroot ${sysroot} ${ios_arch} ${min_ios_sdk} "
    export RANLIB="$(xcrun -sdk ${ios_sdk} -find ranlib) "
    export NM="$(xcrun -sdk ${ios_sdk} -find nm) "
    export OBJDUMP="$(xcrun -sdk ${ios_sdk} -find llvm-objdump) "
    export STRIP="$(xcrun -sdk ${ios_sdk} -find strip) "
    
    export CFLAGS="${OPT_FLAGS}"
    export CXXFLAGS="${OPT_FLAGS}"
    export CCFLAGS="${OPT_FLAGS}"
    export LDFLAGS="-L${sysroot}/usr/lib "
    
    # The preprocessor doesn't work with sysroot set.
    export CPP="$(xcrun -sdk ${ios_sdk} -find cpp) "
    #export CPPFLAGS=""
    
    # A helper to configure.
    alias configure_helper='./configure --host=${target} '
}


iphoneos_armv7_env() {
	ios_sdk="iphoneos"
	target="arm-apple-darwin"
	ios_arch="-arch armv7"
	min_ios_sdk="-mios-version-min=8.0"
	sysroot=$(xcrun -sdk ${ios_sdk} --show-sdk-path)
	setup_build_variables
}

iphoneos_arm64_env(){
	ios_sdk="iphoneos"
	target="arm-apple-darwin"
	ios_arch="-arch arm64"
	min_ios_sdk="-mios-version-min=8.0"
	sysroot=$(xcrun -sdk ${ios_sdk} --show-sdk-path)
	setup_build_variables
}

iphonesimulator_i386_env(){
	ios_sdk="iphonesimulator"
	target="i386-apple-darwin"
	ios_arch="-arch i386"
	min_ios_sdk="-mios-version-min=8.0"
	sysroot=$(xcrun -sdk ${ios_sdk} --show-sdk-path)
	setup_build_variables
}

iphonesimulator_x86_64_env(){
	ios_sdk="iphonesimulator"
	target="x86_64-apple-darwin"
	ios_arch="-arch x86_64"
	min_ios_sdk="-mios-version-min=8.0"
	sysroot=$(xcrun -sdk ${ios_sdk} --show-sdk-path)
	setup_build_variables
}
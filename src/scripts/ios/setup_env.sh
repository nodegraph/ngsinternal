#!/bin/bash

# Run this script after sourcing the macos/setup_env.sh.

# This script takes one argument.
# 1) a number from 1-4 indicating which ios sdk arch to target.

# ARCH specific paths.
if [ $1 == "1" ]; then 
	export IOS_SDK_ARCH="iphoneos_armv7"
elif [ $1 == "2" ]; then
	export IOS_SDK_ARCH="iphoneos_arm64"
elif [ $1 == "3" ]; then
	export IOS_SDK_ARCH="iphonesimulator_i386"
elif [$1 == "4"]; then
	export IOS_SDK_ARCH="iphonesimulator_x86_64"
else
	echo Error: IOS_SDK_ARCH was not specified as argument to script
fi

echo IOS_SDK_ARCH is set to ${IOS_SDK_ARCH}
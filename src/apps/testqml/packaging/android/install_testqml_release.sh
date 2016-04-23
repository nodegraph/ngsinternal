#!/bin/bash

echo "Enter password: "
read STORE_PASS

ninja prep_testqml

@ANDROID_DEPLOY_QT@\
    --verbose\
    --release\
    \
  	--output @CMAKE_BINARY_DIR@/install_testqml\
  	--input @ANDROID_PACKAGE_SOURCE_DIRECTORY@/deployment-settings.json\
  	--android-platform android-23\
  	--deployment bundled\
  	--ant C:/Users/shing/Documents/apps/apache-ant-1.9.6/bin/ant.bat\
  	\
  	--device @ANDROID_DEVICE_ID@\
  	--storepass  $STORE_PASS\
  	--sign C:/Users/shing/Documents/critical/publishing/android/android_release.keystore todopile\
  	
  	
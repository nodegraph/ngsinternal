qt5_add_resources(resources resources.qrc)

if (${ARCH} STREQUAL ARCH_LINUX)
	add_executable(testguiqml main.cpp ${resources})
	target_link_libraries(testguiqml 
		debug Qt5WebEngined optimized Qt5WebEngine 
		debug Qt5Qmld optimized Qt5Qml)
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
	add_executable(testguiqml main.cpp ${resources})
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	target_link_libraries(testguiqml 
		debug Qt5WebEngined optimized Qt5WebEngine 
		debug Qt5Qmld optimized Qt5Qml)
elseif (${ARCH} STREQUAL ARCH_WINRT)
	add_executable(testguiqml main.cpp ${resources})
	target_link_libraries(testguiqml 
		debug Qt5WebViewd optimized Qt5WebView 
		debug Qt5Qmld optimized Qt5Qml)
elseif (${ARCH} STREQUAL ARCH_ANDROID)
	add_library(testguiqml SHARED main.cpp ${resources})
	target_link_libraries(testguiqml gnustl_shared Qt5Qml Qt5WebView)
endif ()

target_link_libraries(testguiqml ngs_memoryallocator ngs_gui_qt ngs_entities)

if(${ARCH} STREQUAL ARCH_ANDROID)
    
else()

endif()

# Install.
INSTALL(TARGETS testguiqml
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT unittests
)


# you may need to install these yum packages
# yum install glibc.i686
# yum install libstdc++.i686
# yum install zlib.i686

# run this from the install dir
# cp -fr /home/raindrop/devandroid/local/Qt5.5.0/5.5/android_armv7/src/android/java/* /tmp/android_test
# cp -fr /home/raindrop/devandroid/nodegraphsoftware-build/install/lib/* /tmp/android_test/libs/armeabi-v7a
# androiddeployqt --input ~/dev/nodegraphsoftware/src/unittests/guiqml/android-deployment-settings.json --verbose --output /tmp/android_test
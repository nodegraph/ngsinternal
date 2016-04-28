qt5_add_resources(resources resources.qrc)

if (${ARCH} STREQUAL ARCH_LINUX)
	add_executable(testguiqml main.cpp ${resources})
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	target_link_libraries(testguiqml 
		debug Qt5WebEngined optimized Qt5WebEngine 
		debug Qt5Qmld optimized Qt5Qml)
elseif (${ARCH} STREQUAL ARCH_MACOS)
	add_executable(testguiqml main.cpp ${resources})
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	target_link_libraries(testguiqml 
		Qt5::WebEngine 
		Qt5::Qml)
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


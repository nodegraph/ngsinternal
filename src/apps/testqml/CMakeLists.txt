qt5_add_resources(resources qml.qrc)

include_directories("${PROJECT_SOURCE_DIR}/apps/testqml")

set(sources
	${resources}
	native/main.cpp
)

set(moc_headers
    )
    
set(headers
)

qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})

if (${ARCH} STREQUAL ARCH_LINUX)
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	add_executable(testqml ${sources})
	target_link_libraries(testqml 
		Qt5::Widgets
		Qt5::Qml 
		)
elseif (${ARCH} STREQUAL ARCH_MACOS)
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	add_executable(testqml MACOSX_BUNDLE ${sources})
	target_link_libraries(testqml 
		Qt5::Widgets
		Qt5::Qml 
		)
elseif (${ARCH} STREQUAL ARCH_IOS)

	include(BundleUtilities)

	# CMake variables which will populate the Info.plist file for the bundle.
	set(MACOSX_BUNDLE_INFO_STRING "testqml")
	set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.octoplier.testqml")
	set(MACOSX_BUNDLE_LONG_VERSION_STRING "testqml Version 0.0.0.1")
	set(MACOSX_BUNDLE_BUNDLE_NAME testqml)
	set(MACOSX_BUNDLE_SHORT_VERSION_STRING testqml)
	set(MACOSX_BUNDLE_BUNDLE_VERSION 0.0.0.1)
	set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2016. All Rights Reserved.")

	file (GLOB icon_images ${CMAKE_CURRENT_SOURCE_DIR}/packaging/ios/icons/*.png)
	file (GLOB launch_images ${CMAKE_CURRENT_SOURCE_DIR}/packaging/ios/launch/*.png)
	
	set_source_files_properties(${icon_images} PROPERTIES MACOSX_PACKAGE_LOCATION "/testqml.app")
	set_source_files_properties(${launch_images} PROPERTIES MACOSX_PACKAGE_LOCATION "/testqml.app")
	
	message("icons are: ${icon_images}")
	message("launch are: ${launch_images}")

	# The target will pick up the bundle settings from above.
	add_executable(testqml MACOSX_BUNDLE ${sources} ${icon_images} ${launch_images})
	
	# Now we set custom Info.plist file, which will override the above settings.
	# We do this just illustrate the different ways to do this.
	set_target_properties(testqml PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/packaging/ios/Info.plist)
	
	target_link_libraries(testqml 
		Qt5::Widgets
		Qt5::Qml 
		Qt5::Quick
		qios_debug
		${ios_libs}
		)
		
	set(CMAKE_EXE_LINKER_FLAGS " -u _qt_registerPlatformPlugin -Wl,-e,_qt_main_wrapper ")
	
	set(resources ${CMAKE_CURRENT_SOURCE_DIR}/packaging/ios/LaunchScreen.storyboard)
	set(ios_sdk_target 8.0)
	set(code_signer "iPhone Developer")
	
	set_target_properties(testqml PROPERTIES RESOURCE "${resources}")
	set_target_properties(testqml PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${ios_sdk_target})
    #set_target_properties(testqml PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${code_signer})
	set_target_properties(testqml PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
	set_target_properties(testqml PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
	
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	add_executable(testqml ${sources})
	target_link_libraries(testqml
		Qt5::Widgets
		Qt5::Qml 
		)
elseif (${ARCH} STREQUAL ARCH_WINRT)
	add_executable(testqml WIN32 ${sources})
	target_link_libraries(testqml  runtimeobject windowscodecs kernel32 ole32
		Qt5::Widgets
		Qt5::Qml 
		debug libEGLd optimized libEGL
		debug libGLESv2d optimized libGLESv2
		)
elseif (${ARCH} STREQUAL ARCH_ANDROID)
    add_library(testqml SHARED ${sources})
	target_link_libraries(testqml gnustl_shared
		Qt5::Widgets
		Qt5::Qml 
		Qt5::AndroidExtras
	)
endif ()

# Install.
INSTALL(TARGETS testqml
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  RESOURCE DESTINATION Resources
  COMPONENT apps
)

if (${ARCH} MATCHES "ARCH_WINRT")
	include("./arch_winrt.cmake")	
elseif (${ARCH} MATCHES "ARCH_ANDROID")
	include("./arch_android.cmake")
elseif (${ARCH} MATCHES "ARCH_MACOS")
	include("./arch_macos.cmake")
elseif (${ARCH} MATCHES "ARCH_IOS")
	include("./arch_ios.cmake")
endif()

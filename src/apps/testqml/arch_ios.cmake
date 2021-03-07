	include(BundleUtilities)
	
	# CMake variables which will populate the Info.plist file for the bundle.
	#set_target_properties(testqml PROPERTIESMACOSX_BUNDLE_INFO_STRING "testqml")
	#set_target_properties(testqml PROPERTIESMACOSX_BUNDLE_GUI_IDENTIFIER "com.youmacro.testqml")
	#set_target_properties(testqml PROPERTIESMACOSX_BUNDLE_LONG_VERSION_STRING "testqml Version 0.0.0.1")
	#set_target_properties(testqml PROPERTIESMACOSX_BUNDLE_BUNDLE_NAME testqml)
	#set_target_properties(testqml PROPERTIESMACOSX_BUNDLE_SHORT_VERSION_STRING testqml)
	#set_target_properties(testqml PROPERTIESMACOSX_BUNDLE_BUNDLE_VERSION 0.0.0.1)
	#set_target_properties(testqml PROPERTIESMACOSX_BUNDLE_COPYRIGHT "Copyright 2016. All Rights Reserved.")

	
	# We use a custom Info.plist file.
	set_target_properties(testqml PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/packaging/ios/Info.plist)
	
	# Extra linker flags for qt main wrapper.
	set(CMAKE_EXE_LINKER_FLAGS " -u _qt_registerPlatformPlugin -Wl,-e,_qt_main_wrapper ")
	
	set(ios_sdk_target 8.0)
	set(code_signer "iPhone Developer")
	
	set_target_properties(testqml PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${ios_sdk_target})
    set_target_properties(testqml PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${code_signer})
	set_target_properties(testqml PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
	set_target_properties(testqml PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
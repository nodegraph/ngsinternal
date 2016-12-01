qt5_add_resources(resources resources.qrc)

if (${ARCH} STREQUAL ARCH_LINUX)
	add_executable(testguiqml main.cpp ${resources})
	target_link_libraries(testguiqml 
		Qt5::Qml)
elseif (${ARCH} STREQUAL ARCH_MACOS)
	add_executable(testguiqml MACOSX_BUNDLE main.cpp ${resources})
	target_link_libraries(testguiqml 
		Qt5::Qml)
elseif (${ARCH} STREQUAL ARCH_IOS)
	add_executable(testguiqml main.cpp ${resources})
	target_link_libraries(testguiqml 
		Qt5::WebView 
		Qt5::Qml
		${ios_libs}
		)
	set_target_properties(testguiqml PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
	add_executable(testguiqml main.cpp ${resources})
	target_link_libraries(testguiqml 
	    ngs_quick
		Qt5::Qml)
elseif (${ARCH} STREQUAL ARCH_WINRT)
	add_executable(testguiqml main.cpp ${resources})
	target_link_libraries(testguiqml 
		Qt5::WebView 
		Qt5::Qml)
elseif (${ARCH} STREQUAL ARCH_ANDROID)
	add_library(testguiqml SHARED main.cpp ${resources})
	target_link_libraries(testguiqml 
		Qt5::WebView 
		Qt5::Qml)
endif ()

target_link_libraries(testguiqml ngs_memoryallocator ngs_gui ngs_entities)

# Install.
INSTALL(TARGETS testguiqml
  RUNTIME DESTINATION bin
  COMPONENT unittests
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
)


# you may need to install these yum packages
# yum install glibc.i686
# yum install libstdc++.i686
# yum install zlib.i686


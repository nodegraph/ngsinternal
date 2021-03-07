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
	add_executable(testqml ${sources})
	target_link_libraries(testqml 
		Qt5::Widgets
		Qt5::Qml 
		)
elseif (${ARCH} STREQUAL ARCH_MACOS)
	add_executable(testqml MACOSX_BUNDLE ${sources})
	target_link_libraries(testqml 
		Qt5::Widgets
		Qt5::Qml 
		)
elseif (${ARCH} STREQUAL ARCH_IOS)
	file (GLOB icon_images ${CMAKE_CURRENT_SOURCE_DIR}/packaging/ios/icons/*.png)
	file (GLOB launch_images ${CMAKE_CURRENT_SOURCE_DIR}/packaging/ios/launch/*.png)
	set_source_files_properties(${icon_images} PROPERTIES MACOSX_PACKAGE_LOCATION "/testqml.app")
	set_source_files_properties(${launch_images} PROPERTIES MACOSX_PACKAGE_LOCATION "/testqml.app")
	#message("icons are: ${icon_images}")
	#message("launch are: ${launch_images}")

	# The target will pick up the bundle settings from above.
	add_executable(testqml MACOSX_BUNDLE ${sources} ${icon_images} ${launch_images})
	target_link_libraries(testqml 
		Qt5::Widgets
		Qt5::Qml 
		Qt5::Quick
		qios_debug
		${ios_libs}
		)
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
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
if (NOT (${ARCH} STREQUAL ARCH_MACOS))
INSTALL(TARGETS testqml
  RUNTIME DESTINATION bin
  COMPONENT unittests
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  RESOURCE DESTINATION Resources
)
endif()

if (${ARCH} MATCHES "ARCH_WINRT")
	include("./arch_winrt.cmake")	
elseif (${ARCH} MATCHES "ARCH_ANDROID")
	include("./arch_android.cmake")
elseif (${ARCH} MATCHES "ARCH_MACOS")
	include("./arch_macos.cmake")
elseif (${ARCH} MATCHES "ARCH_IOS")
	include("./arch_ios.cmake")
endif()

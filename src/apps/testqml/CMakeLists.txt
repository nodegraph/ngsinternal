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
	target_link_libraries(testqml Qt5Qml Qt5WebEngine Qt5Multimedia Qt5OpenGL)
elseif (${ARCH} STREQUAL ARCH_MACOS)
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	add_executable(testqml MACOSX_BUNDLE ${sources})
	target_link_libraries(testqml 
		Qt5::Core
		Qt5::Quick
		Qt5::Widgets
		Qt5::Gui
		Qt5::Qml 
		Qt5::Network
		)
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	add_executable(testqml ${sources})
	target_link_libraries(testqml
		Qt5::Core
		Qt5::Quick
		Qt5::Widgets
		Qt5::Gui
		Qt5::Qml 
		Qt5::Network
		)
elseif (${ARCH} STREQUAL ARCH_WINRT)
	add_executable(testqml WIN32 ${sources})
	target_link_libraries(testqml  runtimeobject windowscodecs kernel32 ole32
		Qt5::Core
		Qt5::Quick
		Qt5::Widgets
		Qt5::Gui
		Qt5::Qml 
		Qt5::Network
		debug libEGLd optimized libEGL
		debug libGLESv2d optimized libGLESv2
		)
elseif (${ARCH} STREQUAL ARCH_ANDROID)
    #list(APPEND sources native/javabridge.cpp)
    #list(APPEND moc_headers2 native/javabridge.h)
    qt5_wrap_cpp(moc_imps2 ${moc_headers2})
    list(APPEND sources ${moc_imps2})
    add_library(testqml SHARED ${sources})
	target_link_libraries(testqml gnustl_shared
		Qt5::Core
		Qt5::Quick
		Qt5::Widgets
		Qt5::Gui
		Qt5::Qml 
		Qt5::Network
		Qt5::AndroidExtras
	)
endif ()

# Install.
INSTALL(TARGETS testqml
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  COMPONENT apps
)

if (${ARCH} MATCHES "ARCH_WINRT")
	include("./arch_winrt.cmake")	
elseif (${ARCH} MATCHES "ARCH_ANDROID")
	include("./arch_android.cmake")
elseif (${ARCH} MATCHES "ARCH_MACOS")
	include("./arch_macos.cmake")
endif()

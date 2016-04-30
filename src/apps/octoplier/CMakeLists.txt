qt5_add_resources(resources qml.qrc)

include_directories("${PROJECT_SOURCE_DIR}/apps/octoplier")

set(sources
	${resources}
	native/app.cpp
	native/main.cpp
	native/scripts.cpp
	native/treeitem.cpp
	native/treemodel.cpp
	native/variantmaptreemodel.cpp
	native/utils.cpp
	native/cppbridge.cpp
)

set(moc_headers
	native/scripts.h
	native/treemodel.h
	native/variantmaptreemodel.h
	native/utils.h
	native/cppbridge.h
    )
    
set(headers
	native/treeitem.h
	native/app.h
)

qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})

if (${ARCH} STREQUAL ARCH_LINUX)
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	add_executable(octoplier ${sources})
	target_link_libraries(octoplier ngs_gui Qt5Qml Qt5WebEngine Qt5Multimedia Qt5OpenGL)
elseif (${ARCH} STREQUAL ARCH_MACOS)
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	add_executable(octoplier MACOSX_BUNDLE ${sources})
	target_link_libraries(octoplier 
		ngs_gui 
		ngs_entities
		Qt5::Widgets
		Qt5::Qml 
    	Qt5::OpenGL
    	Qt5::Gui
    	Qt5::Core
    	Qt5::Quick
		Qt5::WebEngine 
		Qt5::Multimedia
		)
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	add_executable(octoplier ${sources})
	target_link_libraries(octoplier 
		ngs_gui 
		ngs_entities
		Qt5::Widgets
		Qt5::Qml 
    	Qt5::OpenGL
    	Qt5::Gui
    	Qt5::Core
    	Qt5::Quick
		Qt5::WebEngine 
		Qt5::Multimedia
		)
elseif (${ARCH} STREQUAL ARCH_WINRT)
	add_executable(octoplier WIN32 ${sources})
	target_link_libraries(octoplier 
		ngs_gui 
		ngs_entities runtimeobject windowscodecs kernel32 ole32
		Qt5::Widgets
		Qt5::Qml 
		Qt5::OpenGL
		Qt5::Gui
		Qt5::Core
		Qt5::Quick
		Qt5::WebView
		Qt5::Multimedia
		)
elseif (${ARCH} STREQUAL ARCH_ANDROID)
    list(APPEND sources native/javabridge.cpp)
    list(APPEND moc_headers2 native/javabridge.h)
    qt5_wrap_cpp(moc_imps2 ${moc_headers2})
    list(APPEND sources ${moc_imps2})
    add_library(octoplier SHARED ${sources})
	target_link_libraries(octoplier 
		ngs_gui 
		ngs_entities
		Qt5Qml 
		Qt5Multimedia 
		Qt5OpenGL
		Qt5WebView
		Qt5AndroidExtras )
endif ()

# Install.
INSTALL(TARGETS octoplier
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

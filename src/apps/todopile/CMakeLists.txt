qt5_add_resources(resources qml.qrc)

include_directories("${PROJECT_SOURCE_DIR}/apps/todopile")

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
	add_executable(todopile ${sources})
	target_link_libraries(todopile ngs_gui_qt Qt5Qml Qt5WebEngine Qt5Multimedia Qt5OpenGL)
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
	add_definitions("-DQT_WEBVIEW_WEBENGINE_BACKEND")
	add_executable(todopile ${sources})
	target_link_libraries(todopile ngs_gui_qt ngs_entities 
		debug Qt5Qmld optimized Qt5Qml 
		debug Qt5WebEngined optimized Qt5WebEngine 
		#debug Qt5WebView optimized Qt5WebView 
		debug Qt5Multimediad optimized Qt5Multimedia 
		debug Qt5OpenGLd optimized Qt5OpenGL)
elseif (${ARCH} STREQUAL ARCH_WINRT)
	add_executable(todopile WIN32 ${sources})
	target_link_libraries(todopile ngs_gui_qt ngs_entities runtimeobject windowscodecs kernel32 ole32
		debug qtmaind optimized qtmain
		debug Qt5WebViewd optimized Qt5WebView 
		debug Qt5Quickd optimized Qt5Quick 
		debug Qt5WebChanneld optimized Qt5WebChannel
		debug Qt5Qmld optimized Qt5Qml 
		debug Qt5Multimediad optimized Qt5Multimedia 
		debug Qt5OpenGLd optimized Qt5OpenGL)
elseif (${ARCH} STREQUAL ARCH_ANDROID)
    list(APPEND sources native/javabridge.cpp)
    list(APPEND moc_headers2 native/javabridge.h)
    qt5_wrap_cpp(moc_imps2 ${moc_headers2})
    list(APPEND sources ${moc_imps2})
    add_library(todopile SHARED ${sources})
	target_link_libraries(todopile ngs_gui_qt Qt5Qml Qt5Multimedia Qt5OpenGL Qt5AndroidExtras Qt5WebView)
endif ()

# Install.
INSTALL(TARGETS todopile
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT apps
)

if (${ARCH} MATCHES "ARCH_WINRT")
	include("./arch_winrt.cmake")	
elseif (${ARCH} MATCHES "ARCH_ANDROID")
	include("./arch_android.cmake")	
endif()
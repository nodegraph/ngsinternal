qt5_add_resources(resources smashbrowse.qrc)

include_directories("${PROJECT_SOURCE_DIR}/apps/smashbrowse")

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
	add_executable(smashbrowse ${sources})
	target_link_libraries(smashbrowse 
		ngs_gui 
		ngs_entities
		)
elseif (${ARCH} STREQUAL ARCH_MACOS)
	add_executable(smashbrowse MACOSX_BUNDLE ${sources})
	target_link_libraries(smashbrowse 
		ngs_gui 
		ngs_entities
		)
elseif (${ARCH} STREQUAL ARCH_IOS)
	file (GLOB icon_images ${CMAKE_CURRENT_SOURCE_DIR}/packaging/ios/icons/*.png)
	file (GLOB launch_images ${CMAKE_CURRENT_SOURCE_DIR}/packaging/ios/launch/*.png)
	set_source_files_properties(${icon_images} PROPERTIES MACOSX_PACKAGE_LOCATION "/smashbrowse.app")
	set_source_files_properties(${launch_images} PROPERTIES MACOSX_PACKAGE_LOCATION "/smashbrowse.app")
	#message("icons are: ${icon_images}")
	#message("launch are: ${launch_images}")

	# The target will pick up the bundle settings from above.
	add_executable(smashbrowse MACOSX_BUNDLE ${sources} ${icon_images} ${launch_images})
	target_link_libraries(smashbrowse 
		ngs_gui 
		ngs_entities
		Qt5::Quick
		${ios_libs}
		)		
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
    list(APPEND sources packaging/windows/smashbrowse.rc)
    # WIN32 makes the console go away, when running app.
    # When running app directly from the console there will be no output too.
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	    add_executable(smashbrowse ${sources})
	else()
	    add_executable(smashbrowse WIN32 ${sources}) 
	endif()
	target_link_libraries(smashbrowse 
		ngs_gui 
		ngs_entities
		ngs_quick
		)
elseif (${ARCH} STREQUAL ARCH_WINRT)
   if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	    add_executable(smashbrowse ${sources})
	else()
	    add_executable(smashbrowse WIN32 ${sources}) 
	endif()
	target_link_libraries(smashbrowse 
		ngs_gui 
		ngs_entities 
		)
elseif (${ARCH} STREQUAL ARCH_ANDROID)
    list(APPEND sources 
    	native/javabridge.cpp)
    list(APPEND moc_headers2 
    	native/javabridge.h)
    qt5_wrap_cpp(moc_imps2 
    	${moc_headers2})
    add_library(smashbrowse SHARED ${sources} ${moc_imps2})
	target_link_libraries(smashbrowse 
		ngs_gui 
		ngs_entities
		Qt5::Network
		Qt5AndroidExtras )
endif ()


if (${ARCH} MATCHES "ARCH_WINRT")
	include("./arch_winrt.cmake")	
elseif (${ARCH} MATCHES "ARCH_ANDROID")
	include("./arch_android.cmake")
elseif (${ARCH} MATCHES "ARCH_WINDOWS")
	include("./arch_windows.cmake")
elseif (${ARCH} MATCHES "ARCH_MACOS")
	include("./arch_macos.cmake")
elseif (${ARCH} MATCHES "ARCH_IOS")
	include("./arch_ios.cmake")
endif()

add_subdirectory(typescript)
add_subdirectory(java)

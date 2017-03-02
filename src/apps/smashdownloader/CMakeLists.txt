qt5_add_resources(resources ../smashbrowse/smashbrowse.qrc)
qt5_add_resources(resources2 ./smashdownloader.qrc)

include_directories("${PROJECT_SOURCE_DIR}/apps/smashbrowse")

set(sources
	${resources}
	${resources2}
	native/main.cpp
)

set(moc_headers
    )
    
set(headers
)

qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})

if (${ARCH} STREQUAL ARCH_LINUX)
	add_executable(smashdownloader ${sources})
	target_link_libraries(smashdownloader 
		ngs_gui 
		ngs_entities
		)
elseif (${ARCH} STREQUAL ARCH_MACOS)
	# Don't add the path into MACOSX_BUNDLE_ICON_FILE as this goes into the info.plist file.
	set(MACOSX_BUNDLE_ICON_FILE octopus_blue.icns)
	
	# Set a variable to the full path of the icon file.
	set(icon_file ${CMAKE_CURRENT_SOURCE_DIR}/../smashbrowse/packaging/macos/octopus_blue.icns)
	
	# Install it to the Resources dir.
	set_source_files_properties(${icon_file} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
	add_executable(smashdownloader MACOSX_BUNDLE ${icon_file} ${sources} )
	
	target_link_libraries(smashdownloader 
		ngs_gui 
		ngs_entities
		)
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
    list(APPEND sources packaging/windows/smashdownloader.rc)
    # WIN32 makes the console go away, when running app.
    # When running app directly from the console there will be no output too.
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	    add_executable(smashdownloader ${sources})
	else()
	    add_executable(smashdownloader WIN32 ${sources}) 
	endif()
	target_link_libraries(smashdownloader 
		ngs_gui 
		ngs_entities
		ngs_quick
		)
	# Install.
	INSTALL(TARGETS smashdownloader
	  RUNTIME DESTINATION bin
	  COMPONENT smashdownloader
	  LIBRARY DESTINATION lib
	  ARCHIVE DESTINATION lib
	  BUNDLE DESTINATION .
	)
endif ()




if (${ARCH} MATCHES "ARCH_MACOS")
	include("./arch_macos.cmake")
endif()


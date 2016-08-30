qt5_add_resources(resources qml.qrc)

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

# Install.
INSTALL(TARGETS smashbrowse
  RUNTIME DESTINATION bin
  COMPONENT apps
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  
)

# Our nodejs app which controlls webdriver.
INSTALL(DIRECTORY nodejs/
		DESTINATION bin
		COMPONENT apps)

# Install the html files.
INSTALL(DIRECTORY html
		DESTINATION .
		COMPONENT apps)

# Typescript files.
set(chromeextension_src
        typescript/chromeextension/bgcomm.ts
        typescript/chromeextension/bgcommhandler.ts
        typescript/chromeextension/box.ts
        typescript/chromeextension/browserwrap.ts
        typescript/chromeextension/contentcomm.ts
        typescript/chromeextension/contentcommhandler.ts
        typescript/chromeextension/contextmenu.ts
        typescript/chromeextension/contextmenuhandler.ts
        typescript/chromeextension/distinctcolors.ts
        typescript/chromeextension/elemwrap.ts
        typescript/chromeextension/eventblocker.ts
        typescript/chromeextension/matchcriteria.ts
        typescript/chromeextension/overlay.ts
        typescript/chromeextension/overlayset.ts
        typescript/chromeextension/overlaysets.ts
        typescript/chromeextension/mutationmonitor.ts
        typescript/chromeextension/pagewrap.ts
        typescript/chromeextension/guicollection.ts
        typescript/chromeextension/point.ts
        typescript/chromeextension/popupdialog.ts
        typescript/chromeextension/utils.ts
        typescript/chromeextension/contentmain.ts
        typescript/chromeextension/bgmain.ts
)
set(chromeextension_compiled
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/bgcomm.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/bgcommhandler.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/box.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/browserwrap.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/contentcomm.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/contentcommhandler.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/contextmenu.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/contextmenuhandler.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/distinctcolors.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/elemwrap.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/eventblocker.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/matchcriteria.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/overlay.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/overlayset.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/overlaysets.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/mutationmonitor.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/pagewrap.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/guicollection.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/point.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/popupdialog.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/utils.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/contentmain.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/bgmain.js
)
add_custom_command(
    OUTPUT ${chromeextension_compiled}
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR}/typescript/chromeextension --outDir ${CMAKE_CURRENT_BINARY_DIR}/chromeextension
    DEPENDS ${chromeextension_src}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(install_chromeextension ALL DEPENDS ${chromeextension_compiled})

# Install the compiled chrome extension plus the css files.
INSTALL(FILES 
            ${chromeextension_compiled}
            typescript/chromeextension/css/contextmenu.css
            typescript/chromeextension/css/overlay.css
            typescript/chromeextension/css/popupdialog.css
            typescript/chromeextension/manifest.json
		DESTINATION bin/chromeextension
		COMPONENT apps)
		
# Install chrome extension images.
INSTALL(DIRECTORY typescript/chromeextension/images
    DESTINATION bin/chromeextension
COMPONENT apps)


if (${ARCH} MATCHES "ARCH_WINRT")
	include("./arch_winrt.cmake")	
elseif (${ARCH} MATCHES "ARCH_ANDROID")
	include("./arch_android.cmake")
elseif (${ARCH} MATCHES "ARCH_MACOS")
	include("./arch_macos.cmake")
elseif (${ARCH} MATCHES "ARCH_IOS")
	include("./arch_ios.cmake")
endif()

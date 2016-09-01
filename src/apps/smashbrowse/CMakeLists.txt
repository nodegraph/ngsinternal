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
#INSTALL(DIRECTORY nodejs/
#		DESTINATION bin
#		COMPONENT apps)

# Install the html files.
INSTALL(DIRECTORY html
		DESTINATION .
		COMPONENT apps)

# Our nodejs controller files.
set(controller_src
    typescript/controller/debugutils.ts
    typescript/controller/fswrap.ts
    typescript/controller/socketmessage.ts
    typescript/controller/webdriverwrap.ts
    typescript/controller/controller.ts
)
set(controller_compiled
    ${CMAKE_CURRENT_BINARY_DIR}/controller/debugutils.js
    ${CMAKE_CURRENT_BINARY_DIR}/controller/fswrap.js
    ${CMAKE_CURRENT_BINARY_DIR}/controller/socketmessage.js
    ${CMAKE_CURRENT_BINARY_DIR}/controller/webdriverwrap.js
    ${CMAKE_CURRENT_BINARY_DIR}/controller/controller.js
)

# Our chrome extension files.
set(background_src
        typescript/chromeextension/background/browserwrap.ts
        typescript/chromeextension/background/bgcomm.ts
        typescript/chromeextension/background/bgcommhandler.ts
        typescript/chromeextension/background/bgmain.ts
)
set(content_src
        typescript/chromeextension/content/box.ts
        typescript/chromeextension/content/contentcomm.ts
        typescript/chromeextension/content/contentcommhandler.ts
        typescript/chromeextension/content/contextmenu.ts
        typescript/chromeextension/content/contextmenuhandler.ts
        typescript/chromeextension/content/distinctcolors.ts
        typescript/chromeextension/content/elemwrap.ts
        typescript/chromeextension/content/eventblocker.ts
        typescript/chromeextension/content/matchcriteria.ts
        typescript/chromeextension/content/overlay.ts
        typescript/chromeextension/content/overlayset.ts
        typescript/chromeextension/content/overlaysets.ts
        typescript/chromeextension/content/mutationmonitor.ts
        typescript/chromeextension/content/pagewrap.ts
        typescript/chromeextension/content/guicollection.ts
        typescript/chromeextension/content/point.ts
        typescript/chromeextension/content/popupdialog.ts
        typescript/chromeextension/content/utils.ts
        typescript/chromeextension/content/contentmain.ts
)
set(background_compiled
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/bgcomm.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/bgcommhandler.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/box.js
        ${CMAKE_CURRENT_BINARY_DIR}/chromeextension/bgmain.js
)
set(content_compiled
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
)
add_custom_command(
    OUTPUT ${content_compiled} ${background_compiled} ${controller_compiled}
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR}/typescript/chromeextension/background --outDir ${CMAKE_CURRENT_BINARY_DIR}/chromeextension
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR}/typescript/chromeextension/content --outDir ${CMAKE_CURRENT_BINARY_DIR}/chromeextension
    COMMAND tsc
    ARGS --project ${CMAKE_CURRENT_SOURCE_DIR}/typescript/controller --outDir ${CMAKE_CURRENT_BINARY_DIR}/controller
    DEPENDS ${background_src} ${content_src} ${controller_src}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(install_typescript ALL DEPENDS ${content_compiled} ${background_compiled} ${controller_compiled})

# Install the compiled chrome extension plus the css files.
INSTALL(FILES 
            ${background_compiled}
            ${content_compiled}
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

# Install the nodejs controller app.
INSTALL(FILES 
            ${controller_compiled}
            typescript/controller/cert.pem
            typescript/controller/key.pem
		DESTINATION bin
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

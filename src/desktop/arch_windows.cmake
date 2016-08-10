# ------------------------------------------------------------------
# Qt runtime files. 
# ------------------------------------------------------------------

add_custom_command (
  DEPENDS install
  OUTPUT install_desktop_cmd
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testqml.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/testqml/qml 
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/octoplier.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/octoplier/qml 
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testguiqt.exe
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testguiqml.exe
  # copy the nodejs windows install into the bin dir
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/windows_x64/nodejs/" ${CMAKE_BINARY_DIR}/install/bin
  # extra nodejs modules
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/windows_x64/node_modules" ${CMAKE_BINARY_DIR}/install/bin/node_modules
  # chromedriver
  COMMAND ${CMAKE_COMMAND} -E copy "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/windows_x64/chromedriver_2.21/chromedriver.exe" ${CMAKE_BINARY_DIR}/install/bin
)

add_custom_target (install_desktop
   DEPENDS install_desktop_cmd
)

INSTALL(
  DIRECTORY ${QT5_DIR}/plugins/platforms
  DESTINATION bin
  COMPONENT thirdparty
)
INSTALL(
  DIRECTORY ${QT5_DIR}/qml/QtQuick
  DESTINATION bin
  COMPONENT thirdparty
)
INSTALL(
  DIRECTORY ${QT5_DIR}/qml/QtQuick.2
  DESTINATION bin
  COMPONENT thirdparty
)

# Qt debug libraries.
INSTALL(FILES 
			"${QT5_DIR}/bin/icudt54.dll"
			"${QT5_DIR}/bin/icuin54.dll"
			"${QT5_DIR}/bin/icuuc54.dll"
			#"${QT5_DIR}/bin/QtWebEngineProcess.exe"
			#"${QT5_DIR}/plugins/qtwebengine/ffmpegsumo.dll"
		DESTINATION bin
		COMPONENT thirdparty
		CONFIGURATIONS Debug Release)
		
# ------------------------------------------------------------------
# windeployqt misses some libs on windows release
# ------------------------------------------------------------------
		
INSTALL(FILES 
			"${QT5_DIR}/bin/Qt5Cored.dll"
			"${QT5_DIR}/bin/Qt5Widgetsd.dll"
			"${QT5_DIR}/bin/Qt5Guid.dll"
			"${QT5_DIR}/bin/Qt5Quickd.dll"
			"${QT5_DIR}/bin/Qt5Qmld.dll"
			"${QT5_DIR}/bin/Qt5Networkd.dll"
			"${QT5_DIR}/bin/Qt5OpenGLd.dll"
			"${QT5_DIR}/bin/Qt5WebEngined.dll"
			"${QT5_DIR}/bin/Qt5WebEngineCored.dll"
			"${QT5_DIR}/bin/Qt5WebEngineCore.dll"
			"${QT5_DIR}/bin/Qt5WebChanneld.dll"
			"${QT5_DIR}/bin/Qt5Multimediad.dll"
			"${QT5_DIR}/bin/Qt5WebSocketsd.dll"
			
			 # The QtWebEngineProcess is a release binary so we need these also.
			"${QT5_DIR}/bin/Qt5Core.dll"
			"${QT5_DIR}/bin/Qt5Widgets.dll"
			"${QT5_DIR}/bin/Qt5Gui.dll"
			"${QT5_DIR}/bin/Qt5Quick.dll"
			"${QT5_DIR}/bin/Qt5Qml.dll"
			"${QT5_DIR}/bin/Qt5Network.dll"
			"${QT5_DIR}/bin/Qt5OpenGL.dll"
			"${QT5_DIR}/bin/Qt5WebEngine.dll"
			"${QT5_DIR}/bin/Qt5WebEngineCore.dll"
			"${QT5_DIR}/bin/Qt5WebChannel.dll"
			
			"${QT5_DIR}/bin/libGLESv2d.dll"
			
		DESTINATION bin
		COMPONENT thirdparty
		CONFIGURATIONS Debug)
		
INSTALL(FILES 
			"${QT5_DIR}/bin/Qt5Core.dll"
			"${QT5_DIR}/bin/Qt5Widgets.dll"
			"${QT5_DIR}/bin/Qt5Gui.dll"
			"${QT5_DIR}/bin/Qt5Quick.dll"
			"${QT5_DIR}/bin/Qt5Qml.dll"
			"${QT5_DIR}/bin/Qt5Network.dll"
			"${QT5_DIR}/bin/Qt5OpenGL.dll"
			"${QT5_DIR}/bin/Qt5WebEngine.dll"
			"${QT5_DIR}/bin/Qt5WebEngineCore.dll"
			"${QT5_DIR}/bin/Qt5WebChannel.dll"
			"${QT5_DIR}/bin/Qt5Multimedia.dll"
			
			"${QT5_DIR}/bin/libGLESv2.dll"
			
		DESTINATION bin
		COMPONENT thirdparty
		CONFIGURATIONS Release)
		
# QtWebEngine
INSTALL(FILES 
			#"${QT5_DIR}/plugins/qtwebengine/ffmpegsumo.dll"
		DESTINATION qtwebengine
		COMPONENT thirdparty
		CONFIGURATIONS Debug Release)
		
INSTALL(FILES 
			"${QT5_DIR}/bin/Qt5WebEngined.dll"
			"${QT5_DIR}/bin/Qt5WebEngineCored.dll"
			"${QT5_DIR}/bin/Qt5WebChanneld.dll"
			"${QT5_DIR}/bin/Qt5Positioningd.dll"
		DESTINATION bin
		COMPONENT thirdparty
		CONFIGURATIONS Debug)
		
INSTALL(FILES 
			"${QT5_DIR}/bin/Qt5WebEngine.dll"
			"${QT5_DIR}/bin/Qt5WebEngineCore.dll"
			"${QT5_DIR}/bin/Qt5WebChannel.dll"
			"${QT5_DIR}/bin/Qt5Positioning.dll"
		DESTINATION bin
		COMPONENT thirdparty
		CONFIGURATIONS Release)

# ------------------------------------------------------------------
# GLEW	
# ------------------------------------------------------------------

#INSTALL(FILES 
#			"${PLATFORM_ROOT}/srcdeps/ngsexternal/vs2013/glew-1.13.0/lib/debugMX/glew32mxd.dll"
#		DESTINATION bin
#		COMPONENT thirdparty
#		CONFIGURATIONS Debug)
#INSTALL(FILES 
#			"${PLATFORM_ROOT}/srcdeps/ngsexternal/vs2013/glew-1.13.0/lib/releaseMX/glew32mx.dll"
#		DESTINATION bin
#		COMPONENT thirdparty
#		CONFIGURATIONS Release)

# ------------------------------------------------------------------
# Simple Packaging
# ------------------------------------------------------------------
add_custom_command(
    OUTPUT pack_desktop_cmd
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_BINARY_DIR}/desktop.zip" --format=zip ${CMAKE_BINARY_DIR}/install
    )

add_custom_target (pack_desktop
   DEPENDS pack_desktop_cmd
)

# ------------------------------------------------------------------
# Desktop Packaging
# ------------------------------------------------------------------


if(WIN32)
    # On Windows generate MSI packages
    set(CPACK_GENERATOR "WIX")
    
    set(CPACK_PACKAGE_NAME "node_graph_debug")
    set(CPACK_PACKAGE_VERSION "0.0.0.7")

    set(CPACK_WIX_TEMPLATE "${PROJECT_SOURCE_DIR}/desktop/wix.template.in")
    set(CPACK_WIX_UPGRADE_GUID A5BE780A-779E-49CF-8D0D-E6413224710E)
    set(CPACK_WIX_PRODUCT_ICON ${PROJECT_SOURCE_DIR}/desktop/Paomedia-Small-N-Flat-Layers.ico)
    set(CPACK_WIX_LICENSE_RTF ${PROJECT_SOURCE_DIR}/desktop/eula.rtf)
    set(CPACK_WIX_UI_BANNER  ${PROJECT_SOURCE_DIR}/desktop/installer_banner_493x58.png)
    set(CPACK_WIX_UI_DIALOG  ${PROJECT_SOURCE_DIR}/desktop/installer_bg_493x312.png)
    
    #set(CPACK_WIX_PROGRAM_MENU_FOLDER "node graph")

elseif(APPLE)
    # APPLE is also UNIX, so must check for APPLE before UNIX
    SET(CPACK_GENERATOR "DragNDrop")
elseif(UNIX)
    # On Linux generate TAR.GZ,DEB and RPM packages
    #Find out what architecture are we running on and set the package architecture 
endif()

set(CPACK_COMPONENT_THIRDPARTY_DISPLAY_NAME "Basic unit tests to confirm various things are working as expected.")
set(CPACK_COMPONENT_BASE_DISPLAY_NAME "Base libraries which everything depends on.")
set(CPACK_COMPONENT_COMPONENTS_DISPLAY_NAME "Components used by all apps.")
set(CPACK_COMPONENT_GUI_DISPLAY_NAME "Gui components used by graphical apps.")
set(CPACK_COMPONENT_UNITTESTS_DISPLAY_NAME "Basic unit tests to confirm various things are working as expected.")
set(CPACK_COMPONENT_APPS_DISPLAY_NAME "Apps that were build on the node graph framework.")

set(CPACK_COMPONENT_THIRDPARTY_REQUIRED true)
set(CPACK_COMPONENT_BASE_REQUIRED true)
set(CPACK_COMPONENT_COMPONENTS_REQUIRED true)
set(CPACK_COMPONENT_GUI_REQUIRED true)
set(CPACK_COMPONENT_UNITTESTS_REQUIRED true)
set(CPACK_COMPONENT_APPS_REQUIRED true)

include(CPack)

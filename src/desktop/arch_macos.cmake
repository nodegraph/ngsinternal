
# ------------------------------------------------------------------
# Qt runtime files. 
# ------------------------------------------------------------------

add_custom_command (
  #DEPENDS install
  OUTPUT install_desktop_cmd
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testqml.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/testqml/qml 
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/octoplier.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/octoplier/qml 
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testguiqt.exe
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testguiqml.exe
)

add_custom_target (install_desktop
   DEPENDS install_desktop_cmd
)

#INSTALL(
#  DIRECTORY ${QT5_DIR}/plugins/platforms
#  DESTINATION bin
#  COMPONENT thirdparty
#)
#INSTALL(
#  DIRECTORY ${QT5_DIR}/qml/QtQuick
#  DESTINATION bin
#  COMPONENT thirdparty
#)
#INSTALL(
#  DIRECTORY ${QT5_DIR}/qml/QtQuick.2
#  DESTINATION bin
#  COMPONENT thirdparty
#)

# Qt debug libraries.
#INSTALL(FILES 
#			"${QT5_DIR}/bin/icudt54.dll"
#			"${QT5_DIR}/bin/icuin54.dll"
#			"${QT5_DIR}/bin/icuuc54.dll"
#			#"${QT5_DIR}/bin/QtWebEngineProcess.exe"
#			#"${QT5_DIR}/plugins/qtwebengine/ffmpegsumo.dll"
#		DESTINATION bin
#		COMPONENT thirdparty
#		CONFIGURATIONS Debug Release)
		
# ------------------------------------------------------------------
# windeployqt misses some libs on windows release
# ------------------------------------------------------------------
		
#INSTALL(FILES 
#			"${QT5_DIR}/bin/Qt5Cored.dll"
#			"${QT5_DIR}/bin/Qt5Widgetsd.dll"
#			"${QT5_DIR}/bin/Qt5Guid.dll"
#			"${QT5_DIR}/bin/Qt5Quickd.dll"
#			"${QT5_DIR}/bin/Qt5Qmld.dll"
#			"${QT5_DIR}/bin/Qt5Networkd.dll"
#			"${QT5_DIR}/bin/Qt5OpenGLd.dll"
#		DESTINATION bin
#		COMPONENT thirdparty
#		CONFIGURATIONS Debug)
		
#INSTALL(FILES 
#			"${QT5_DIR}/bin/Qt5Core.dll"
#			"${QT5_DIR}/bin/Qt5Widgets.dll"
#			"${QT5_DIR}/bin/Qt5Gui.dll"
#			"${QT5_DIR}/bin/Qt5Quick.dll"
#			"${QT5_DIR}/bin/Qt5Qml.dll"
#			"${QT5_DIR}/bin/Qt5Network.dll"
#			"${QT5_DIR}/bin/Qt5OpenGL.dll"
#		DESTINATION bin
#		COMPONENT thirdparty
#		CONFIGURATIONS Release)
		
# QtWebEngine
#INSTALL(FILES 
#			#"${QT5_DIR}/plugins/qtwebengine/ffmpegsumo.dll"
#		DESTINATION qtwebengine
#		COMPONENT thirdparty
#		CONFIGURATIONS Debug Release)
		
#INSTALL(FILES 
#			"${QT5_DIR}/bin/Qt5WebEngined.dll"
#			"${QT5_DIR}/bin/Qt5WebEngineCored.dll"
#			"${QT5_DIR}/bin/Qt5WebChanneld.dll"
#			"${QT5_DIR}/bin/Qt5Positioningd.dll"
#		DESTINATION bin
#		COMPONENT thirdparty
#		CONFIGURATIONS Debug)
		
#INSTALL(FILES 
#			"${QT5_DIR}/bin/Qt5WebEngine.dll"
#			"${QT5_DIR}/bin/Qt5WebEngineCore.dll"
#			"${QT5_DIR}/bin/Qt5WebChannel.dll"
#			"${QT5_DIR}/bin/Qt5Positioning.dll"
#		DESTINATION bin
#		COMPONENT thirdparty
#		CONFIGURATIONS Release)

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



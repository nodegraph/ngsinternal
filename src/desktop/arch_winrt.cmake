# ------------------------------------------------------------------
# Qt runtime files. 
# ------------------------------------------------------------------

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
		
# ------------------------------------------------------------------
# windeployqt misses some libraries on winrt, so we add them here
# ------------------------------------------------------------------
		
INSTALL(FILES 
			"${QT5_DIR}/bin/Qt5Cored.dll"
			"${QT5_DIR}/bin/Qt5Widgetsd.dll"
			"${QT5_DIR}/bin/Qt5Guid.dll"
			"${QT5_DIR}/bin/Qt5Quickd.dll"
			"${QT5_DIR}/bin/Qt5Qmld.dll"
			"${QT5_DIR}/bin/Qt5Networkd.dll"
			"${QT5_DIR}/bin/Qt5OpenGLd.dll"
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
		DESTINATION bin
		COMPONENT thirdparty
		CONFIGURATIONS Release)
		

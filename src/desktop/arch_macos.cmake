
# ------------------------------------------------------------------
# Qt runtime files. 
# ------------------------------------------------------------------

add_custom_command (
  #DEPENDS install
  OUTPUT install_desktop_cmd
  COMMAND echo dummy spot reserving
  #COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testqml.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/testqml/qml 
  #COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/smashbrowse.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/smashbrowse/qml 
  #COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testguiqt.exe
  #COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testguiqml.exe
)

add_custom_target (install_desktop
   DEPENDS install_desktop_cmd
)

# ------------------------------------------------------------------
# ChromeDriver executable.
# ------------------------------------------------------------------

INSTALL(
    FILES 
        "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/macos/chromedriver_2.27/chromedriver"
    DESTINATION "../build/smashbrowse.app/Contents/MacOS"
    COMPONENT thirdparty
    CONFIGURATIONS Debug Release
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# ------------------------------------------------------------------
# Youtube-dl executable.
# ------------------------------------------------------------------

INSTALL(
  FILES
      "${PLATFORM_ROOT}/srcdeps/ngsexternal/youtube-dl/macos/2017.01.31/youtube-dl"
  RENAME "ngs_helper"
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
  DESTINATION "../build/smashbrowse.app/Contents/MacOS"
  COMPONENT thirdparty
  CONFIGURATIONS Debug Release
)

# ------------------------------------------------------------------
# Ffmpeg executables.
# ------------------------------------------------------------------

INSTALL(
  FILES
      "${PLATFORM_ROOT}/srcdeps/ngsexternal/ffmpeg/macos/83288/ffmpeg"
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
  DESTINATION "../build/smashbrowse.app/Contents/MacOS"
  COMPONENT thirdparty
  CONFIGURATIONS Debug Release
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



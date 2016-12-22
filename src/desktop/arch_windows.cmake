# ------------------------------------------------------------------
# Using 7zip to create a self extracting executable.
# ------------------------------------------------------------------
# 1) Use 7zip file manager to create a .7z file at Ultra LZMA2 settings.
# 2) Copy config.txt to that dir.
# 3) Download "7zSD extra" from 7zip and extract it. 
# 4) Now use the windows cmd shell to do the following.
#    shell<<  copy /b 7zsd_LZMA2_x64.sfx + config.txt + install.7z "smashbrowse.exe"



# Note that WIX seems to have a 255 char limit for paths.
# This applies to the _CPack_Packages directory it builds internally when packing.
# So we need to try to generate our cmake build directory at the shallowest level possible.
# ------------------------------------------------------------------
# Qt Plugins.
# ------------------------------------------------------------------

# Note each of these directories hold debug, release and .pdb files which gets quite big.
# If you need these plugins, you should selectively install the specific dlls you need.
INSTALL(
  DIRECTORY 
      #"${QT5_DIR}/plugins/platforms"
      #"${QT5_DIR}/plugins/qmltooling"
      #"${QT5_DIR}/plugins/bearer"
      #"${QT5_DIR}/plugins/iconengines"
      #"${QT5_DIR}/plugins/imageformats"
      #"${QT5_DIR}/plugins/platforminputcontexts"
  DESTINATION bin
  COMPONENT thirdparty
)

# ------------------------------------------------------------------
# QML Libraries.
# ------------------------------------------------------------------
# Note each of these directories hold debug, release and .pdb files which gets quite big.
# If you need these plugins, you should selectively install the specific dlls you need.
INSTALL(
  DIRECTORY 
      #"${QT5_DIR}/qml/Qt"
      #"${QT5_DIR}/qml/QtGraphicalEffects"
      #"${QT5_DIR}/qml/QtQml"
      #"${QT5_DIR}/qml/QtQuick"
      #"${QT5_DIR}/qml/QtQuick.2"
      #"${QT5_DIR}/qml/QtWebView"
  DESTINATION bin
  COMPONENT thirdparty
)

# ------------------------------------------------------------------
# OpenSSL Libraries.
# ------------------------------------------------------------------

INSTALL(
  FILES
      "${OPENSSL_DIR}/bin/libeay32.dll"
      "${OPENSSL_DIR}/bin/ssleay32.dll"
  DESTINATION bin
  COMPONENT thirdparty
)


# ------------------------------------------------------------------
# NodeJS libraries.
# ------------------------------------------------------------------

INSTALL(
    DIRECTORY 
         "${PLATFORM_ROOT}/srcdeps/ngsexternal/nodejs2/node_modules"
    DESTINATION bin
    COMPONENT thirdparty
    CONFIGURATIONS Debug Release
)

INSTALL(
    FILES 
        "${PLATFORM_ROOT}/srcdeps/ngsexternal/nodejs2/node.exe"
        "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/windows_x64/chromedriver_2.25/chromedriver.exe"
    DESTINATION bin
    COMPONENT thirdparty
    CONFIGURATIONS Debug Release
)

# ------------------------------------------------------------------
# OpenGL Libraries from Qt.
# ------------------------------------------------------------------
INSTALL(
  FILES "${QT5_DIR}/bin/libEGLd.dll"
        "${QT5_DIR}/bin/libGLESv2d.dll"
        "${QT5_DIR}/bin/Qt5OpenGLd.dll"
  DESTINATION bin
  COMPONENT thirdparty
  CONFIGURATIONS Debug 
)

INSTALL(
  FILES "${QT5_DIR}/bin/libEGL.dll"
        "${QT5_DIR}/bin/libGLESv2.dll"
        "${QT5_DIR}/bin/Qt5OpenGL.dll"
  DESTINATION bin
  COMPONENT thirdparty
  CONFIGURATIONS Release
)
		
# ------------------------------------------------------------------
# Main Qt libraries.
# ------------------------------------------------------------------
		
INSTALL(FILES 
			"${QT5_DIR}/bin/Qt5Cored.dll"
			"${QT5_DIR}/bin/Qt5Widgetsd.dll"
			"${QT5_DIR}/bin/Qt5Guid.dll"
			"${QT5_DIR}/bin/Qt5Quickd.dll"
			"${QT5_DIR}/bin/Qt5Qmld.dll"
			"${QT5_DIR}/bin/Qt5Networkd.dll"
			"${QT5_DIR}/bin/Qt5WebSocketsd.dll"			
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
			"${QT5_DIR}/bin/Qt5WebSockets.dll"			
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
# windeployqt custom command.
# ------------------------------------------------------------------

add_custom_command (
  DEPENDS install
  OUTPUT install_desktop_cmd
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testqml.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/testqml/qml 
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/smashbrowse.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/smashbrowse/qml 
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testguiqt.exe
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testguiqml.exe
  # copy the nodejs windows install into the bin dir
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/windows_x64/nodejs/" ${CMAKE_BINARY_DIR}/install/bin
  # extra nodejs modules
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/windows_x64/node_modules" ${CMAKE_BINARY_DIR}/install/bin/node_modules
  # chromedriver
  COMMAND ${CMAKE_COMMAND} -E copy "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/windows_x64/chromedriver_2.25/chromedriver.exe" ${CMAKE_BINARY_DIR}/install/bin
)

add_custom_target (install_desktop
   DEPENDS install_desktop_cmd
)

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

# To build the msi installer for debug binaries, type the following in the msys2 shell.
# cpack -G WIX -c Debug

if(WIN32)
    # On Windows generate MSI packages
    set(CPACK_GENERATOR "WIX")
    
    set(CPACK_PACKAGE_NAME "Smash Browse")
    set(CPACK_PACKAGE_VERSION ${ngs_version})
    set(CPACK_PACKAGE_VENDOR "Node Graph Software")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Smash Browse")

    set(CPACK_WIX_TEMPLATE "${PROJECT_SOURCE_DIR}/desktop/wix.template.in")
    set(CPACK_WIX_UPGRADE_GUID A5BE780A-779E-49CF-8D0D-E6413224710E)
    set(CPACK_WIX_PRODUCT_ICON ${PROJECT_SOURCE_DIR}/external/images/octopus_blue.ico)
    set(CPACK_WIX_LICENSE_RTF ${PROJECT_SOURCE_DIR}/desktop/eula.rtf)
    set(CPACK_WIX_UI_BANNER  ${PROJECT_SOURCE_DIR}/desktop/installer_banner_493x58.png)
    set(CPACK_WIX_UI_DIALOG  ${PROJECT_SOURCE_DIR}/desktop/installer_bg_493x312.png)
    
    set(CPACK_WIX_UNINSTALL "1")
    
    # Hacks to shorten the build/packaging paths.
    set(CPACK_PACKAGE_DIRECTORY "p")
    set(CPACK_PACKAGE_FILE_NAME "b")
    
    #set(CPACK_PACKAGE_EXECUTABLES "bin/smashbrowse" "Smash Browse" ${CPACK_PACKAGE_EXECUTABLES})
    #set(CPACK_CREATE_DESKTOP_LINKS "smashbrowse" ${CPACK_CREATE_DESKTOP_LINKS})
    

    set(CPACK_WIX_PROGRAM_MENU_FOLDER "Smash Browse")
    
    set_property(INSTALL "bin/smashbrowse.exe"
        PROPERTY CPACK_DESKTOP_SHORTCUTS "Smash Browse"
    )
    set_property(INSTALL "bin/smashbrowse.exe"
        PROPERTY CPACK_START_MENU_SHORTCUTS "Smash Browse"
    )

elseif(APPLE)
    # APPLE is also UNIX, so must check for APPLE before UNIX
    SET(CPACK_GENERATOR "DragNDrop")
elseif(UNIX)
    # On Linux generate TAR.GZ,DEB and RPM packages
    #Find out what architecture are we running on and set the package architecture 
endif()


#set(CPACK_COMPONENT_THIRDPARTY_DISPLAY_NAME "Other libraries. (Required)")
#set(CPACK_COMPONENT_BASE_DISPLAY_NAME "Base libraries. (Required)")
#set(CPACK_COMPONENT_COMPONENTS_DISPLAY_NAME "Non Gui Components. (Required)")
#set(CPACK_COMPONENT_GUI_DISPLAY_NAME "Gui components. (Required)")
#set(CPACK_COMPONENT_UNITTESTS_DISPLAY_NAME "Basic unit tests. (Optional)")
#set(CPACK_COMPONENT_APPS_DISPLAY_NAME "Apps. (Required)")

#set(CPACK_COMPONENT_THIRDPARTY_REQUIRED true)
#set(CPACK_COMPONENT_BASE_REQUIRED true)
#set(CPACK_COMPONENT_COMPONENTS_REQUIRED true)
#set(CPACK_COMPONENT_GUI_REQUIRED true)
#set(CPACK_COMPONENT_UNITTESTS_REQUIRED false)
#set(CPACK_COMPONENT_APPS_REQUIRED true)

#set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL thirdparty base components gui apps)

include(CPack)

cpack_add_component(thirdparty
                    DISPLAY_NAME "Third party libraries."
                    HIDDEN REQUIRED)
                    
cpack_add_component(base
                    DISPLAY_NAME "Base libraries."
                    HIDDEN REQUIRED)
                    
cpack_add_component(components
                    DISPLAY_NAME "Non GUI components."
                    HIDDEN REQUIRED)
                    
cpack_add_component(gui
                    DISPLAY_NAME "GUI components."
                    HIDDEN REQUIRED)
                    
cpack_add_component(unittests
                    DISPLAY_NAME "Unit tests."
                    HIDDEN REQUIRED)
                    
cpack_add_component(apps
                    DISPLAY_NAME "Application binaries."
                    DESCRIPTION "These are the main app binaries."
                    HIDDEN REQUIRED)

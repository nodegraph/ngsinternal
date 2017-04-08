# ------------------------------------------------------------------
# Using 7zip to create a self extracting executable.
# ------------------------------------------------------------------
# 1) Use 7zip file manager to create a .7z file at Ultra LZMA2 settings.
# 2) Copy config.txt to that dir.
# 3) Download "7zSD extra" from 7zip and extract it. 
# 4) Now use the windows cmd shell to do the following.
#    shell<<  copy /b 7zsd_LZMA2_x64.sfx + config.txt + install.7z "youmacro.exe"



# Note that WIX seems to have a 255 char limit for paths.
# This applies to the _CPack_Packages directory it builds internally when packing.
# So we need to try to generate our cmake build directory at the shallowest level possible.
# ------------------------------------------------------------------
# Qt Plugins.
# ------------------------------------------------------------------

# Note each of these directories hold debug, release and .pdb files which gets quite big.
# We currently only weed out the pdb files from installation.
INSTALL(
  DIRECTORY 
      "${QT5_DIR}/plugins/platforms"
      "${QT5_DIR}/plugins/qmltooling"
      "${QT5_DIR}/plugins/bearer"
      "${QT5_DIR}/plugins/iconengines"
      "${QT5_DIR}/plugins/imageformats"
      #"${QT5_DIR}/plugins/platforminputcontexts"
  DESTINATION bin
  COMPONENT thirdparty
  PATTERN "*.pdb" EXCLUDE
)

# ------------------------------------------------------------------
# QML Libraries.
# ------------------------------------------------------------------
# Note each of these directories hold debug, release and .pdb files which gets quite big.
# We currently only weed out the pdb files from installation.
INSTALL(
  DIRECTORY 
      "${QT5_DIR}/qml/Qt"
      "${QT5_DIR}/qml/QtGraphicalEffects"
      "${QT5_DIR}/qml/QtQml"
      "${QT5_DIR}/qml/QtQuick"
      "${QT5_DIR}/qml/QtQuick.2"
      #"${QT5_DIR}/qml/QtWebView"
  DESTINATION bin
  COMPONENT thirdparty
  PATTERN "*.pdb" EXCLUDE
)

# ------------------------------------------------------------------
# LibSodium Libraries.
# ------------------------------------------------------------------

if (${ARCH_BITS} STREQUAL "x64")
	INSTALL(FILES 
		"${LIBSODIUM_DIR}/x64/${CMAKE_BUILD_TYPE}/v140/dynamic/libsodium.dll"
		DESTINATION bin
		COMPONENT thirdparty
		CONFIGURATIONS Debug Release)
else()
	INSTALL(FILES 
		"${LIBSODIUM_DIR}/Win32/${CMAKE_BUILD_TYPE}/v140/dynamic/libsodium.dll"
		DESTINATION bin
		COMPONENT thirdparty
		CONFIGURATIONS Debug Release)
endif()

# ------------------------------------------------------------------
# OpenSSL Libraries.
# ------------------------------------------------------------------

INSTALL(
  FILES
      "${OPENSSL_DIR}/bin/libeay32.dll"
      "${OPENSSL_DIR}/bin/ssleay32.dll"
      "C:/Program Files (x86)/Windows Kits/10/bin/x64/ucrt/ucrtbased.dll"
      "C:/Program Files (x86)/Windows Kits/10/Redist/ucrt/DLLs/x64/ucrtbase.dll"
  DESTINATION bin
  COMPONENT thirdparty
)


# ------------------------------------------------------------------
# Youtube-dl executable.
# ------------------------------------------------------------------

INSTALL(
  FILES
      "${PLATFORM_ROOT}/srcdeps/ngsexternal/youtube-dl/windows/2017.02.28/youtube-dl.exe"
  RENAME "ngs_helper.dll"
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
  DESTINATION bin
  COMPONENT thirdparty
  CONFIGURATIONS Debug Release
)

# ------------------------------------------------------------------
# Ffmpeg executables.
# ------------------------------------------------------------------

INSTALL(
  FILES
      "${PLATFORM_ROOT}/srcdeps/ngsexternal/ffmpeg/windows/2017.01.30/ffmpeg.exe"
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
  DESTINATION bin
  COMPONENT thirdparty
  CONFIGURATIONS Debug Release
)

# ------------------------------------------------------------------
# ChromeDriver executable.
# ------------------------------------------------------------------

INSTALL(
    FILES 
        "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/windows_x64/chromedriver_2.28/chromedriver.exe"
    DESTINATION bin
    COMPONENT thirdparty_chromedriver
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
			"${QT5_DIR}/bin/Qt5QuickControls2d.dll"
			"${QT5_DIR}/bin/Qt5QuickTemplates2d.dll"		
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
			"${QT5_DIR}/bin/Qt5QuickControls2.dll"
			"${QT5_DIR}/bin/Qt5QuickTemplates2.dll"		
		DESTINATION bin
		COMPONENT thirdparty
		CONFIGURATIONS Release)
		
# ------------------------------------------------------------------
# GSON. Json library for java.
# ------------------------------------------------------------------
		
INSTALL(
  DIRECTORY
      "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/gson"
  DESTINATION .
  COMPONENT thirdparty_java
)

# ------------------------------------------------------------------
# Selenium Java Library.
# ------------------------------------------------------------------

INSTALL(
  DIRECTORY 
      "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/"
  DESTINATION selenium
  COMPONENT thirdparty_java
)
		
# ------------------------------------------------------------------
# JRE. Install the version that's already installed on the dev machine.
# ------------------------------------------------------------------
# We don't install the JRE anymore as it increases the binary size considerably.
# Also most people won't be using it.
	
#INSTALL(DIRECTORY 
#			"C:/Program Files/Java/jre1.8.0_102/"
#		DESTINATION jre
#		USE_SOURCE_PERMISSIONS
#		COMPONENT thirdparty_java
#		CONFIGURATIONS Debug Release		
#		)
		
# ------------------------------------------------------------------
# MSVC Redistributable.
# ------------------------------------------------------------------
# We don't install the vc runtime anymore in order to keep the download small.

#INSTALL(FILES
#			"${PLATFORM_ROOT}/srcdeps/ngsexternal/vc_runtime/2015_update_3/vc_redist.x64.exe"
#		DESTINATION bin
#		COMPONENT thirdparty
#		CONFIGURATIONS Debug Release)
		
# ------------------------------------------------------------------
# Icon for shortcut on windows.
# ------------------------------------------------------------------
INSTALL(FILES
			${PROJECT_SOURCE_DIR}/external/images/robot_blue.ico
		DESTINATION bin
		COMPONENT thirdparty
		CONFIGURATIONS Debug Release)
		
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
# Qt windeployqt Packaging.
# ------------------------------------------------------------------

#add_custom_command (
#  DEPENDS install
#  OUTPUT install_desktop_cmd
#  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testqml.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/testqml/qml 
#  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/youmacro.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/youmacro/qml 
#  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testguiqt.exe
#  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install/bin/testguiqml.exe
#  # chromedriver
#  COMMAND ${CMAKE_COMMAND} -E copy "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/windows_x64/chromedriver_2.28/chromedriver.exe" ${CMAKE_BINARY_DIR}/install/bin
#)

#add_custom_target (install_desktop
#   DEPENDS install_desktop_cmd
#)

# ------------------------------------------------------------------
# Simple ZIP Packaging
# ------------------------------------------------------------------
add_custom_command(
    OUTPUT pack_desktop_cmd
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_BINARY_DIR}/desktop.zip" --format=zip ${CMAKE_BINARY_DIR}/install
    )

add_custom_target (pack_desktop
   DEPENDS pack_desktop_cmd
)

# ------------------------------------------------------------------
# WIX Tools Packaging
# ------------------------------------------------------------------

# To build the msi installer for debug binaries, type the following in the msys2 shell.
# cpack -G WIX -c Debug

function(create_cpack_config filename) 
  set(CPACK_OUTPUT_CONFIG_FILE "${filename}") 
  include(CPack) 
endfunction(create_cpack_config) 

set(package_type "youmacro")

if(WIN32)
    # On Windows generate MSI packages
    if (${package_type} STREQUAL youmacro)
        set(CPACK_GENERATOR "WIX")
        set(CPACK_PACKAGE_NAME "YouMacro")
        set(CPACK_PACKAGE_VERSION ${ngs_version})
        set(CPACK_PACKAGE_VENDOR "Node Graph Software")
        set(CPACK_PACKAGE_INSTALL_DIRECTORY "YouMacro")
        
        set(CPACK_WIX_TEMPLATE "${PROJECT_SOURCE_DIR}/desktop/wix.template.in")
        set(CPACK_WIX_UPGRADE_GUID 1F015D65-E1C1-468F-A2A5-4E431E279F70)
        set(CPACK_WIX_PRODUCT_ICON ${PROJECT_SOURCE_DIR}/external/images/robot_blue.ico)
        set(CPACK_WIX_LICENSE_RTF ${PROJECT_SOURCE_DIR}/desktop/eula.rtf)
        set(CPACK_WIX_UI_BANNER  ${PROJECT_SOURCE_DIR}/desktop/installer_banner_493x58.png)
        set(CPACK_WIX_UI_DIALOG  ${PROJECT_SOURCE_DIR}/desktop/installer_bg_493x312.png)
        
        set(CPACK_WIX_UNINSTALL "1")
        
        set(CPACK_PACKAGE_DIRECTORY "d:/b1installers/youmacro")
        
        set(CPACK_WIX_PROGRAM_MENU_FOLDER "YouMacro")
        
        set_property(INSTALL "bin/youmacro.exe"
            PROPERTY CPACK_DESKTOP_SHORTCUTS "YouMacro"
        )
        set_property(INSTALL "bin/youmacro.exe"
            PROPERTY CPACK_START_MENU_SHORTCUTS "YouMacro"
        )
        set(CPACK_COMPONENTS_ALL
            youmacro
            chrome_ext_background
            chrome_ext_content
            jcomm
            gui
            components 
            base
            thirdparty
            thirdparty_java
            thirdparty_chromedriver
        )
    endif()
    
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


include(CPack)

cpack_add_component(youmacro
                    DISPLAY_NAME "YouMacro Application."
                    HIDDEN REQUIRED)
                    
cpack_add_component(chrome_ext_background
                    DISPLAY_NAME "Chrome extension background."
                    HIDDEN REQUIRED)

cpack_add_component(chrome_ext_content
                    DISPLAY_NAME "Chrome extension content."
                    HIDDEN REQUIRED)

cpack_add_component(jcomm
                    DISPLAY_NAME "Java communication."
                    HIDDEN REQUIRED)

cpack_add_component(gui
                    DISPLAY_NAME "GUI components."
                    HIDDEN REQUIRED)

cpack_add_component(components
                    DISPLAY_NAME "Non GUI components."
                    HIDDEN REQUIRED)

cpack_add_component(base
                    DISPLAY_NAME "Base libraries."
                    HIDDEN REQUIRED)

cpack_add_component(thirdparty
                    DISPLAY_NAME "Third party libraries."
                    HIDDEN REQUIRED)
                    
cpack_add_component(thirdparty_java
                    DISPLAY_NAME "Third party java libraries."
                    HIDDEN REQUIRED)
                    
cpack_add_component(thirdparty_chromedriver
                    DISPLAY_NAME "Third party chromedriver."
                    HIDDEN REQUIRED)
                    
cpack_add_component(unittests
                    DISPLAY_NAME "Unit tests."
                    HIDDEN REQUIRED)
                    
                    
                    
                    
                    

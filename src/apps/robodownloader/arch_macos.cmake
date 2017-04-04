# To install do.
# ninja install
# ninja fill_robodownloader

# If robodownloader.app doesn't seem to be updating then do.
# ninja wipe_robodownloader
# ninja install
# ninja fill_robodownloader

# Command to install dependent libraries into the the app.
set(app ${CMAKE_CURRENT_BINARY_DIR}/robodownloader.app/Contents)
add_custom_command(
	OUTPUT copy_other_files
	# We need to append the flag -always-overwrite but there is a bug where the
	# the libs won't be copied properly, so we have to manually remove files/folders.
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/robodownloader.app/Contents/Frameworks
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/robodownloader.app/Contents/Plugins
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/robodownloader.app/Contents/Resources
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/robodownloader.app/Contents/Resources
	COMMAND cp ${CMAKE_SOURCE_DIR}/external/images/robot_blue.icns ${CMAKE_CURRENT_BINARY_DIR}/robodownloader.app/Contents/Resources
	COMMAND ${QT5_DIR}/bin/macdeployqt ARGS ${CMAKE_CURRENT_BINARY_DIR}/robodownloader.app -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/qml -verbose=3 
	#
	COMMAND cp -fRL ${CMAKE_CURRENT_SOURCE_DIR}/html ${app}/Resources
	COMMAND mkdir -p ${app}/Resources/jre
	COMMAND cp -fRL /Users/raindrop/installs/macosunpacks/jre1.8.0_121.jre/Contents/Home/bin ${app}/Resources/jre
	COMMAND cp -fRL /Users/raindrop/installs/macosunpacks/jre1.8.0_121.jre/Contents/Home/lib ${app}/Resources/jre
	COMMAND chmod -R +rw ${app}/Resources/jre/bin
	COMMAND chmod -R +rw ${app}/Resources/jre/lib
	COMMAND mkdir -p ${app}/Resources/selenium
	COMMAND cp -fRL ${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/* ${app}/Resources/selenium
	#
	COMMAND cp -fRL "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/gson" ${app}/Resources/gson
	COMMAND cp -fRL ${CMAKE_BINARY_DIR}/install/chromeextension ${app}/Resources/chromeextension
	COMMAND cp -fRL ${CMAKE_BINARY_DIR}/install/bin ${app}/Resources
	COMMAND cp -fRL ${CMAKE_BINARY_DIR}/install/appmacros ${app}/Resources
)


# Custom target which fills out robodownloader with other dependencies and files.
add_custom_target (fill_robodownloader
   DEPENDS copy_other_files
)		

add_custom_command(
	OUTPUT wipe_app_bundle
	# Note that the info.plist in robodownloader.app/Contents is only created at CMAKE configure time.
	# So we need to keep that file.
	COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/robodownloader.app/Contents/Info.plist ${CMAKE_CURRENT_BINARY_DIR}/.
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/robodownloader.app
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/robodownloader.app/Contents
	COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/Info.plist ${CMAKE_CURRENT_BINARY_DIR}/robodownloader.app/Contents
)

add_custom_target (wipe_robodownloader
   DEPENDS wipe_app_bundle
)

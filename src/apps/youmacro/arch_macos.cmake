# To install do.
# ninja install
# ninja fill_youmacro

# If youmacro.app doesn't seem to be updating then do.
# ninja wipe_youmacro
# ninja install
# ninja fill_youmacro

# Command to install dependent libraries into the the app.
set(app ${CMAKE_CURRENT_BINARY_DIR}/youmacro.app/Contents)
add_custom_command(
	OUTPUT copy_other_files
	# We need to append the flag -always-overwrite but there is a bug where the
	# the libs won't be copied properly, so we have to manually remove files/folders.
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/youmacro.app/Contents/Frameworks
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/youmacro.app/Contents/Plugins
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/youmacro.app/Contents/Resources
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/youmacro.app/Contents/Resources
	COMMAND cp ${CMAKE_SOURCE_DIR}/external/images/robot_blue.icns ${CMAKE_CURRENT_BINARY_DIR}/youmacro.app/Contents/Resources
	COMMAND ${QT5_DIR}/bin/macdeployqt ARGS ${CMAKE_CURRENT_BINARY_DIR}/youmacro.app -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/qml -verbose=3 
	# Copy html files.
	COMMAND cp -fRL ${CMAKE_CURRENT_SOURCE_DIR}/html ${app}/Resources
	# Copy JRE
	COMMAND mkdir -p ${app}/Resources/jre
	COMMAND cp -fRL /Users/raindrop/installs/macosunpacks/jre1.8.0_121.jre/Contents/Home/bin ${app}/Resources/jre
	COMMAND cp -fRL /Users/raindrop/installs/macosunpacks/jre1.8.0_121.jre/Contents/Home/lib ${app}/Resources/jre
	COMMAND chmod -R +rw ${app}/Resources/jre/bin
	COMMAND chmod -R +rw ${app}/Resources/jre/lib
	# Copy Selenium
	COMMAND mkdir -p ${app}/Resources/selenium
	COMMAND cp -fRL ${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.3.1/* ${app}/Resources/selenium
	# Copy GSON.
	COMMAND cp -fRL "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/gson" ${app}/Resources/gson
	# Copy chrome extension.
	COMMAND cp -fRL ${CMAKE_BINARY_DIR}/install/chromeextension ${app}/Resources/chromeextension
	# Copy bin executables.
	COMMAND cp -fRL ${CMAKE_BINARY_DIR}/install/bin ${app}/Resources
	# Copy app macros.
	COMMAND cp -fRL ${CMAKE_BINARY_DIR}/install/appmacros ${app}/Resources
)


# Custom target which fills out youmacro with other dependencies and files.
add_custom_target (fill_youmacro
   DEPENDS copy_other_files
)		

add_custom_command(
	OUTPUT wipe_app_bundle
	# Note that the info.plist in youmacro.app/Contents is only created at CMAKE configure time.
	# So we need to keep that file.
	COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/youmacro.app/Contents/Info.plist ${CMAKE_CURRENT_BINARY_DIR}/.
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/youmacro.app
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/youmacro.app/Contents
	COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/Info.plist ${CMAKE_CURRENT_BINARY_DIR}/youmacro.app/Contents
)

add_custom_target (wipe_youmacro
   DEPENDS wipe_app_bundle
)

# To install do.
# ninja install
# ninja fill_smashdownloader

# If smashbrowse.app doesn't seem to be updating then do.
# ninja wipe_smashdownloader
# ninja install
# ninja fill_smashdownloader

# Command to install dependent libraries into the the app.
set(app ${CMAKE_CURRENT_BINARY_DIR}/smashdownloader.app/Contents)
add_custom_command(
	OUTPUT fill_smashdownloader_cmd
	# We need to append the flag -always-overwrite but there is a bug where the
	# the libs won't be copied properly, so we have to manually remove files/folders.
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/smashdownloader.app/Contents/Frameworks
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/smashdownloader.app/Contents/Plugins
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/smashdownloader.app/Contents/Resources
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/smashdownloader.app/Contents/Resources
	COMMAND cp ${CMAKE_SOURCE_DIR}/external/images/octopus_blue.icns ${CMAKE_CURRENT_BINARY_DIR}/smashdownloader.app/Contents/Resources
	COMMAND ${QT5_DIR}/bin/macdeployqt ARGS ${CMAKE_CURRENT_BINARY_DIR}/smashdownloader.app -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/qml -verbose=3
	COMMAND mkdir -p ${app}/Resources
	COMMAND cp -fRL ${CMAKE_BINARY_DIR}/install/bin ${app}/Resources
)

# Custom target which fills out smashdownloader with other dependencies and files.
add_custom_target (fill_smashdownloader
   DEPENDS fill_smashdownloader_cmd
)	

add_custom_command(
	OUTPUT wipe_smashdownloader_cmd
	COMMAND rm -fr ${CMAKE_CURRENT_BINARY_DIR}/smashbrowse.app
)

add_custom_target (wipe_smashdownloader
   DEPENDS wipe_app_bundle
)


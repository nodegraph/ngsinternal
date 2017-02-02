# Note to build os macos do the following:
# 1) ninja
# 2) ninja install
# 3) ninja bundle_smashdownloader
# 4) ninja package_smashdownloader

add_custom_command (
	OUTPUT bundle_smashdownloader_cmd
	
	# Copy the bin dir into the bundle.
	COMMAND cp -fr ${CMAKE_BINARY_DIR}/install/bin/* ${SMASHDOWNLOADER_BIN_DIR}/.
)

add_custom_target (bundle_smashdownloader
   DEPENDS bundle_smashdownloader_cmd
)

add_custom_command (
	OUTPUT package_smashdownloader_cmd
	
	# Remove previous dmg files.
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/smashdownloader.dmg
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/smashdownloader.dmg.shadow
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/smashdownloader_app.dmg
	
	
	# Note we want macdeployqt to strip our binaries However it seems to produce stripped binary of our old code.
	# So instead we have wipe out the build dir, and call ninja install. This makes macdeployqt produce the right result.
	#COMMAND sudo rm -fr ${CMAKE_BINARY_DIR}/build
	#COMMAND cd ${CMAKE_BINARY_DIR}
	#COMMAND ninja install
	#COMMAND cd ${CMAKE_BINARY_DIR}
	
	COMMAND ${QT5_DIR}/bin/macdeployqt ${CMAKE_BINARY_DIR}/build/smashdownloader.app -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/qml -dmg -verbose=3
	
	# Update the dmg with an application folder and icons.
	COMMAND ${CMAKE_SOURCE_DIR}/apps/shared/update_dmg.sh 
				${CMAKE_BINARY_DIR}/build/smashdownloader.dmg 
				${CMAKE_BINARY_DIR}/build/smashdownloader_app.dmg 
				${CMAKE_SOURCE_DIR}/external/images/octopus_blue.icns
				smashdownloader
)

add_custom_target (package_smashdownloader
   DEPENDS package_smashdownloader_cmd
)


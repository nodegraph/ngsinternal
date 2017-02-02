add_custom_command (
	OUTPUT bundle_smashbrowse_cmd
	
	# Copy the bin dir into the bundle.
	COMMAND cp -fr ${CMAKE_BINARY_DIR}/install/bin/* ${SMASHBROWSE_BIN_DIR}/.
)

add_custom_target (bundle_smashbrowse
   DEPENDS bundle_smashbrowse_cmd
)

add_custom_command (
	OUTPUT package_smashbrowse_cmd
	
	# Remove previous dmg files.
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/smashbrowse.dmg
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/smashbrowse.dmg.shadow
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/smashbrowse_app.dmg
	
	
	# Note we want macdeployqt to strip our binaries However it seems to produce stripped binary of our old code.
	# So instead we have wipe out the build dir, and call ninja install. This makes macdeployqt produce the right result.
	#COMMAND sudo rm -fr ${CMAKE_BINARY_DIR}/build
	#COMMAND cd ${CMAKE_BINARY_DIR}
	#COMMAND ninja install
	#COMMAND cd ${CMAKE_BINARY_DIR}
	
	COMMAND ${QT5_DIR}/bin/macdeployqt ${CMAKE_BINARY_DIR}/build/smashbrowse.app -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/qml -dmg -verbose=3
	
	# Update the dmg with an application folder and icons.
	COMMAND ${CMAKE_SOURCE_DIR}/apps/shared/update_dmg.sh 
				${CMAKE_BINARY_DIR}/build/smashbrowse.dmg 
				${CMAKE_BINARY_DIR}/build/smashbrowse_app.dmg 
				${CMAKE_SOURCE_DIR}/external/images/octopus_blue.icns
				smashbrowse
)

add_custom_target (package_smashbrowse
   DEPENDS package_smashbrowse_cmd
)


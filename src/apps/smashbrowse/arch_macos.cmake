

add_custom_command (
	OUTPUT install_smashbrowse_cmd
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/smashbrowse.dmg
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/smashbrowse.dmg.shadow
	COMMAND ${QT5_DIR}/bin/macdeployqt ${CMAKE_BINARY_DIR}/build/smashbrowse.app -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/qml -dmg -verbose=3 
	COMMAND ${CMAKE_SOURCE_DIR}/apps/shared/update_dmg.sh 
				${CMAKE_BINARY_DIR}/build/smashbrowse.dmg 
				${CMAKE_BINARY_DIR}/build/smashbrowse_pimped.dmg 
				${CMAKE_SOURCE_DIR}/external/images/octopus_blue.icns
				smashbrowse
)

add_custom_target (install_smashbrowse
   DEPENDS install_smashbrowse_cmd
)


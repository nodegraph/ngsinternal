

add_custom_command (
	OUTPUT install_octoplier_cmd
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/octoplier.dmg
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/octoplier.dmg.shadow
	COMMAND ${QT5_DIR}/bin/macdeployqt ${CMAKE_BINARY_DIR}/build/octoplier.app -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/qml -dmg -verbose=3 
	COMMAND ${CMAKE_SOURCE_DIR}/apps/shared/update_dmg.sh 
				${CMAKE_BINARY_DIR}/build/octoplier.dmg 
				${CMAKE_BINARY_DIR}/build/octoplier_pimped.dmg 
				${CMAKE_SOURCE_DIR}/external/images/octopus_blue.icns
				octoplier
)

add_custom_target (install_octoplier
   DEPENDS install_octoplier_cmd
)


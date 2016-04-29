
add_custom_command (
	OUTPUT install_octoplier_cmd
	COMMAND ${QT5_DIR}/bin/macdeployqt ${CMAKE_BINARY_DIR}/build/octoplier.app -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/qml -dmg -verbose=3 
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/update_dmg.sh ${CMAKE_BINARY_DIR}/build/octoplier.dmg ${CMAKE_BINARY_DIR}/build/octoplier_pimped.dmg ${CMAKE_SOURCE_DIR}/external/images/octoplier.icns
)

add_custom_target (install_octoplier
   DEPENDS install_octoplier_cmd
)


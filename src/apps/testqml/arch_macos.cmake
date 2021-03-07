

add_custom_command (
	OUTPUT install_testqml_cmd
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/testqml.dmg
	COMMAND rm -f ${CMAKE_BINARY_DIR}/build/testqml.dmg.shadow
	COMMAND ${QT5_DIR}/bin/macdeployqt ${CMAKE_BINARY_DIR}/build/testqml.app -qmldir=${CMAKE_CURRENT_SOURCE_DIR}/qml -dmg -verbose=3 -always-overwrite
	COMMAND ${CMAKE_SOURCE_DIR}/apps/shared/update_dmg.sh 
				${CMAKE_BINARY_DIR}/build/testqml.dmg 
				${CMAKE_BINARY_DIR}/build/testqml_app.dmg 
				${CMAKE_SOURCE_DIR}/external/images/robot_blue.icns
				testqml
)

add_custom_target (install_testqml
   DEPENDS install_testqml_cmd
)
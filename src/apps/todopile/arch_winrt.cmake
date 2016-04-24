# Make our main install_todopile output dir.
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/install_todopile)

add_custom_command (
  DEPENDS install
  OUTPUT install_todopile_cmd
  # Copy the install files over to install_todopile.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/install ${CMAKE_BINARY_DIR}/install_todopile
  # Copy our AppxManifext.xml file over.
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/winrt_x64_debug.xml ${CMAKE_BINARY_DIR}/install_todopile/bin
  # Rename it.
  COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/install_todopile/bin/winrt_x64_debug.xml ${CMAKE_BINARY_DIR}/install_todopile/bin/AppxManifest.xml
  # Make the asset dir.
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/install_todopile/bin/assets
  # Copy our assets over.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/assets ${CMAKE_BINARY_DIR}/install_todopile/bin/assets
  COMMAND ${QT5_DIR}/bin/windeployqt --debug --verbose 3 --angle ${CMAKE_BINARY_DIR}/install_todopile/bin/todopile.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/todopile/qml 
)

add_custom_target (install_todopile
   DEPENDS install_todopile_cmd
)

add_custom_command (
	OUTPUT run_todopile_cmd
	COMMAND ${QT5_DIR}/bin/winrtrunner --device 0 --start --stop --install --wait 0 --profile appx ${CMAKE_BINARY_DIR}/install_todopile/bin/todopile.exe
)

add_custom_target (run_todopile
   DEPENDS run_todopile_cmd
)

add_custom_command (
	OUTPUT pack_todopile_cmd
	COMMAND MakeAppx pack /d ${CMAKE_BINARY_DIR}/install_todopile/bin /p ${CMAKE_BINARY_DIR}/install_todopile/todopile.appx /o /v
	COMMAND echo "**********************************************************************************************" 
	COMMAND echo "you must use the following command to sign the appx package but the PASSWORD with the real one" 
	COMMAND echo " - not you must use it from a raw visual studio command shell " 
	COMMAND echo "**********************************************************************************************" 
	COMMAND echo signtool sign /fd sha256 /f ${SRC_ROOT}/publishing/winrt/MyKey.pfx /p PASSWORD ${CMAKE_BINARY_DIR}/install_todopile/todopile.appx
	)
	
add_custom_target (pack_todopile
   DEPENDS pack_todopile_cmd
)



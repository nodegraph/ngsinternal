# Make our main install_todopile output dir.
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/install_testqml)


add_custom_command (
  DEPENDS install
  OUTPUT install_testqml_cmd
  # Copy the install files over to install_testqml.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/install ${CMAKE_BINARY_DIR}/install_testqml
  # Copy our AppxManifext.xml file over.
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/winrt_x64_debug.xml ${CMAKE_BINARY_DIR}/install_testqml/bin
  # Rename it.
  COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/install_testqml/bin/winrt_x64_debug.xml ${CMAKE_BINARY_DIR}/install_testqml/bin/AppxManifest.xml
  # Make the asset dir.
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/install_testqml/bin/assets
  # Copy our assets over.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/assets ${CMAKE_BINARY_DIR}/install_testqml/bin/assets
  COMMAND ${QT5_DIR}/bin/windeployqt --debug --verbose 3 --angle ${CMAKE_BINARY_DIR}/install_testqml/bin/testqml.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/testqml/qml 
)

add_custom_target (install_testqml
   DEPENDS install_testqml_cmd
)

add_custom_command (
	OUTPUT run_testqml_cmd
    COMMAND ${QT5_DIR}/bin/winrtrunner --device 0 --start --stop --install --wait 0 --profile appx ${CMAKE_BINARY_DIR}/install_testqml/bin/testqml.exe
)

add_custom_target (run_testqml
   DEPENDS run_testqml_cmd
)

add_custom_command (
	OUTPUT pack_testqml_cmd
	COMMAND MakeAppx pack /d ${CMAKE_BINARY_DIR}/install_testqml/bin /p ${CMAKE_BINARY_DIR}/install_testqml/testqml.appx
	)
	
add_custom_target (pack_testqml
   DEPENDS pack_testqml_cmd
)

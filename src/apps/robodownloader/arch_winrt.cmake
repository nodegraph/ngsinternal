# Make our main install_robodownloader output dir.
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/install_robodownloader)

add_custom_command (
  DEPENDS install
  OUTPUT install_robodownloader_cmd
  # Copy the install files over to install_robodownloader.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/install ${CMAKE_BINARY_DIR}/install_robodownloader
  # Copy our AppxManifext.xml file over.
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/winrt_${ARCH_BITS}_${build_type}.xml ${CMAKE_BINARY_DIR}/install_robodownloader/bin
  # Rename it.
  COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/install_robodownloader/bin/winrt_${ARCH_BITS}_${build_type}.xml ${CMAKE_BINARY_DIR}/install_robodownloader/bin/AppxManifest.xml
  # Make the asset dir.
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/install_robodownloader/bin/assets
  # Copy our assets over.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/assets ${CMAKE_BINARY_DIR}/install_robodownloader/bin/assets
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install_robodownloader/bin/robodownloader.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/robodownloader/qml 
)

add_custom_target (install_robodownloader
   DEPENDS install_robodownloader_cmd
)

add_custom_command (
	OUTPUT run_robodownloader_cmd
	COMMAND ${QT5_DIR}/bin/winrtrunner --device 0 --start --stop --install --wait 0 --profile appx ${CMAKE_BINARY_DIR}/install_robodownloader/bin/robodownloader.exe
)

add_custom_target (run_robodownloader
   DEPENDS run_robodownloader_cmd
)

add_custom_command (
	OUTPUT unregister_robodownloader_cmd
	COMMAND ${QT5_DIR}/bin/winrtrunner --device 0 --remove --profile appx ${CMAKE_BINARY_DIR}/install_robodownloader/bin/robodownloader.exe
)

add_custom_target (unregister_robodownloader
   DEPENDS unregister_robodownloader_cmd
)

add_custom_command (
	OUTPUT pack_robodownloader_cmd
	COMMAND MakeAppx pack /d ${CMAKE_BINARY_DIR}/install_robodownloader/bin /p ${CMAKE_BINARY_DIR}/install_robodownloader/robodownloader.appx /o /v
	COMMAND signtool sign /fd sha256 /f ${PLATFORM_ROOT}/srcdeps/publishing/winrt/MyKey.pfx /p ${PASSWORD} ${CMAKE_BINARY_DIR}/install_robodownloader/robodownloader.appx
	)
	
add_custom_target (pack_robodownloader
   DEPENDS pack_robodownloader_cmd
)



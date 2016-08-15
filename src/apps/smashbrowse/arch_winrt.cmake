# Make our main install_smashbrowse output dir.
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/install_smashbrowse)

add_custom_command (
  DEPENDS install
  OUTPUT install_smashbrowse_cmd
  # Copy the install files over to install_smashbrowse.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/install ${CMAKE_BINARY_DIR}/install_smashbrowse
  # Copy our AppxManifext.xml file over.
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/winrt_${ARCH_BITS}_${build_type}.xml ${CMAKE_BINARY_DIR}/install_smashbrowse/bin
  # Rename it.
  COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/install_smashbrowse/bin/winrt_${ARCH_BITS}_${build_type}.xml ${CMAKE_BINARY_DIR}/install_smashbrowse/bin/AppxManifest.xml
  # Make the asset dir.
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/install_smashbrowse/bin/assets
  # Copy our assets over.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/assets ${CMAKE_BINARY_DIR}/install_smashbrowse/bin/assets
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install_smashbrowse/bin/smashbrowse.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/smashbrowse/qml 
)

add_custom_target (install_smashbrowse
   DEPENDS install_smashbrowse_cmd
)

add_custom_command (
	OUTPUT run_smashbrowse_cmd
	COMMAND ${QT5_DIR}/bin/winrtrunner --device 0 --start --stop --install --wait 0 --profile appx ${CMAKE_BINARY_DIR}/install_smashbrowse/bin/smashbrowse.exe
)

add_custom_target (run_smashbrowse
   DEPENDS run_smashbrowse_cmd
)

add_custom_command (
	OUTPUT unregister_smashbrowse_cmd
	COMMAND ${QT5_DIR}/bin/winrtrunner --device 0 --remove --profile appx ${CMAKE_BINARY_DIR}/install_smashbrowse/bin/smashbrowse.exe
)

add_custom_target (unregister_smashbrowse
   DEPENDS unregister_smashbrowse_cmd
)

add_custom_command (
	OUTPUT pack_smashbrowse_cmd
	COMMAND MakeAppx pack /d ${CMAKE_BINARY_DIR}/install_smashbrowse/bin /p ${CMAKE_BINARY_DIR}/install_smashbrowse/smashbrowse.appx /o /v
	COMMAND signtool sign /fd sha256 /f ${PLATFORM_ROOT}/srcdeps/publishing/winrt/MyKey.pfx /p ${PASSWORD} ${CMAKE_BINARY_DIR}/install_smashbrowse/smashbrowse.appx
	)
	
add_custom_target (pack_smashbrowse
   DEPENDS pack_smashbrowse_cmd
)



# Make our main install_octoplier output dir.
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/install_octoplier)

add_custom_command (
  DEPENDS install
  OUTPUT install_octoplier_cmd
  # Copy the install files over to install_octoplier.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/install ${CMAKE_BINARY_DIR}/install_octoplier
  # Copy our AppxManifext.xml file over.
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/winrt_${ARCH_BITS}_${build_type}.xml ${CMAKE_BINARY_DIR}/install_octoplier/bin
  # Rename it.
  COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/install_octoplier/bin/winrt_${ARCH_BITS}_${build_type}.xml ${CMAKE_BINARY_DIR}/install_octoplier/bin/AppxManifest.xml
  # Make the asset dir.
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/install_octoplier/bin/assets
  # Copy our assets over.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/assets ${CMAKE_BINARY_DIR}/install_octoplier/bin/assets
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install_octoplier/bin/octoplier.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/octoplier/qml 
)

add_custom_target (install_octoplier
   DEPENDS install_octoplier_cmd
)

add_custom_command (
	OUTPUT run_octoplier_cmd
	COMMAND ${QT5_DIR}/bin/winrtrunner --device 0 --start --stop --install --wait 0 --profile appx ${CMAKE_BINARY_DIR}/install_octoplier/bin/octoplier.exe
)

add_custom_target (run_octoplier
   DEPENDS run_octoplier_cmd
)

add_custom_command (
	OUTPUT unregister_octoplier_cmd
	COMMAND ${QT5_DIR}/bin/winrtrunner --device 0 --remove --wait 0 --profile appx ${CMAKE_BINARY_DIR}/install_octoplier/bin/octoplier.exe
)

add_custom_target (unregister_octoplier
   DEPENDS unregister_octoplier_cmd
)

add_custom_command (
	OUTPUT pack_octoplier_cmd
	COMMAND MakeAppx pack /d ${CMAKE_BINARY_DIR}/install_octoplier/bin /p ${CMAKE_BINARY_DIR}/install_octoplier/octoplier.appx /o /v
	COMMAND signtool sign /fd sha256 /f ${PLATFORM_ROOT}/srcdeps/publishing/winrt/MyKey.pfx /p ${PASSWORD} ${CMAKE_BINARY_DIR}/install_octoplier/octoplier.appx
	)
	
add_custom_target (pack_octoplier
   DEPENDS pack_octoplier_cmd
)



# Make our main install_youmacro output dir.
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/install_youmacro)

add_custom_command (
  DEPENDS install
  OUTPUT install_youmacro_cmd
  # Copy the install files over to install_youmacro.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/install ${CMAKE_BINARY_DIR}/install_youmacro
  # Copy our AppxManifext.xml file over.
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/winrt_${ARCH_BITS}_${build_type}.xml ${CMAKE_BINARY_DIR}/install_youmacro/bin
  # Rename it.
  COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/install_youmacro/bin/winrt_${ARCH_BITS}_${build_type}.xml ${CMAKE_BINARY_DIR}/install_youmacro/bin/AppxManifest.xml
  # Make the asset dir.
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/install_youmacro/bin/assets
  # Copy our assets over.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/packaging/winrt/assets ${CMAKE_BINARY_DIR}/install_youmacro/bin/assets
  COMMAND ${QT5_DIR}/bin/windeployqt --${build_type} --verbose 3 --angle ${CMAKE_BINARY_DIR}/install_youmacro/bin/youmacro.exe --qmldir ${CMAKE_SOURCE_DIR}/apps/youmacro/qml 
)

add_custom_target (install_youmacro
   DEPENDS install_youmacro_cmd
)

add_custom_command (
	OUTPUT run_youmacro_cmd
	COMMAND ${QT5_DIR}/bin/winrtrunner --device 0 --start --stop --install --wait 0 --profile appx ${CMAKE_BINARY_DIR}/install_youmacro/bin/youmacro.exe
)

add_custom_target (run_youmacro
   DEPENDS run_youmacro_cmd
)

add_custom_command (
	OUTPUT unregister_youmacro_cmd
	COMMAND ${QT5_DIR}/bin/winrtrunner --device 0 --remove --profile appx ${CMAKE_BINARY_DIR}/install_youmacro/bin/youmacro.exe
)

add_custom_target (unregister_youmacro
   DEPENDS unregister_youmacro_cmd
)

add_custom_command (
	OUTPUT pack_youmacro_cmd
	COMMAND MakeAppx pack /d ${CMAKE_BINARY_DIR}/install_youmacro/bin /p ${CMAKE_BINARY_DIR}/install_youmacro/youmacro.appx /o /v
	COMMAND signtool sign /fd sha256 /f ${PLATFORM_ROOT}/srcdeps/publishing/winrt/MyKey.pfx /p ${PASSWORD} ${CMAKE_BINARY_DIR}/install_youmacro/youmacro.appx
	)
	
add_custom_target (pack_youmacro
   DEPENDS pack_youmacro_cmd
)



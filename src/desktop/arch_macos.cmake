# ------------------------------------------------------------------
# ChromeDriver executable.
# ------------------------------------------------------------------

INSTALL(
    FILES 
        "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/macos/chromedriver_2.27/chromedriver"
    DESTINATION "../build/smashbrowse.app/Contents/MacOS"
    COMPONENT thirdparty
    CONFIGURATIONS Debug Release
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# ------------------------------------------------------------------
# Youtube-dl executable.
# ------------------------------------------------------------------

INSTALL(
  FILES
      "${PLATFORM_ROOT}/srcdeps/ngsexternal/youtube-dl/macos/2017.01.31/youtube-dl"
  RENAME "ngs_helper"
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
  DESTINATION "../build/smashbrowse.app/Contents/MacOS"
  COMPONENT thirdparty
  CONFIGURATIONS Debug Release
)

# ------------------------------------------------------------------
# Ffmpeg executables.
# ------------------------------------------------------------------

INSTALL(
  FILES
      "${PLATFORM_ROOT}/srcdeps/ngsexternal/ffmpeg/macos/83288/ffmpeg"
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
  DESTINATION "../build/smashbrowse.app/Contents/MacOS"
  COMPONENT thirdparty
  CONFIGURATIONS Debug Release
)

# ------------------------------------------------------------------
# Simple ZIP Packaging
# ------------------------------------------------------------------
add_custom_command(
    OUTPUT pack_desktop_cmd
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_BINARY_DIR}/desktop.zip" --format=zip ${CMAKE_BINARY_DIR}/install
    )

add_custom_target (pack_desktop
   DEPENDS pack_desktop_cmd
)



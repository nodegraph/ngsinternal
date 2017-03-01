

# ------------------------------------------------------------------
# ChromeDriver executable.
# ------------------------------------------------------------------

INSTALL(
    FILES 
        "${PLATFORM_ROOT}/srcdeps/ngsexternal/browsercontroller/macos/chromedriver_2.27/chromedriver"
    DESTINATION bin
    COMPONENT thirdparty
    CONFIGURATIONS Debug Release
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# ------------------------------------------------------------------
# LibSodium Libraries.
# ------------------------------------------------------------------

INSTALL(FILES 
	"${LIBSODIUM_DIR}/lib/libsodium.18.dylib"
	DESTINATION lib
	COMPONENT thirdparty
	CONFIGURATIONS Debug Release)

# ------------------------------------------------------------------
# Youtube-dl executable.
# ------------------------------------------------------------------

INSTALL(
  FILES
      "${PLATFORM_ROOT}/srcdeps/ngsexternal/youtube-dl/macos/2017.02.28/youtube-dl"
  RENAME "ngs_helper"
  PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ
  DESTINATION bin
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
  DESTINATION bin
  COMPONENT thirdparty
  CONFIGURATIONS Debug Release
)

# ------------------------------------------------------------------
# GSON. Json library for java.
# ------------------------------------------------------------------
		
#INSTALL(
#  DIRECTORY
#      "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/gson"
#  DESTINATION bin
#  COMPONENT thirdparty
#)

# ------------------------------------------------------------------
# Selenium Java Library.
# ------------------------------------------------------------------

#INSTALL(
#  DIRECTORY 
#      "${PLATFORM_ROOT}/srcdeps/ngsexternal/java/selenium-java-3.0.1/"
#  DESTINATION selenium
#  COMPONENT thirdparty
#)

# ------------------------------------------------------------------
# JRE. Install the version that's already installed on the dev machine.
# ------------------------------------------------------------------
		
#INSTALL(DIRECTORY 
#	"/Users/raindrop/installs/macosunpacks/jre1.8.0_121.jre/Contents/Home/"
#	#"/Library/Java/JavaVirtualMachines/jdk1.8.0_92.jdk/Contents/Home/jre"
#	DESTINATION jre
#	USE_SOURCE_PERMISSIONS
#	COMPONENT thirdparty
#	CONFIGURATIONS Debug Release		
#)

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



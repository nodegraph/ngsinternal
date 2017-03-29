# This directories provides extra components required for 
# distribution to various target platforms. 


INSTALL(
  DIRECTORY
      "${PLATFORM_ROOT}/srcdeps/ngsexternal/appmacros"
  DESTINATION .
  COMPONENT robodownloader
)

if (${ARCH} MATCHES "ARCH_WINDOWS")
	include("./arch_windows.cmake")	
elseif (${ARCH} MATCHES "ARCH_MACOS")
	include("./arch_macos.cmake")	
elseif (${ARCH} MATCHES "ARCH_WINRT")
	include("./arch_winrt.cmake")	
endif()

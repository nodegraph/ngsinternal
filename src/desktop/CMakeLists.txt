# This directories provides extra components required for 
# distribution to various target platforms. 

if (${ARCH} MATCHES "ARCH_WINDOWS")
	include("./arch_windows.cmake")	
elseif (${ARCH} MATCHES "ARCH_WINRT")
	include("./arch_winrt.cmake")	
endif()
# Third party header libraries.
# These have not been modified.
# boost 1.57.0 headers
# glm 0.9.5.4 headers
# chromium key codes header
 
# Third party libraries that we have modified.
#add_subdirectory(iGLU-1.0.0)
#add_subdirectory(ftgl)
add_subdirectory(half)
#add_subdirectory(freetype-gl)
 
if (${ARCH} MATCHES "ARCH_LINUX")
elseif (${ARCH} MATCHES "ARCH_WINDOWS")
elseif (${ARCH} MATCHES "ARCH_WINRT")
elseif (${ARCH} MATCHES "ARCH_ANDROID")
endif ()


#INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glm-0.9.5.4/glm DESTINATION include)

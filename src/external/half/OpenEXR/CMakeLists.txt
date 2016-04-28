# ---------------------------------------------------------------------
# half - extracted from ilmbase
# ---------------------------------------------------------------------
set(sources
    half.cpp
    toFloat.cpp
	)
	
set(headers
    half.h
    eLut.h
    halfExport.h
    halfFunction.h
    halfLimits.h
    toFloat.h
	)
	
add_library(ngs_half ${sources} ${headers})
generate_export_header(ngs_half EXPORT_MACRO_NAME HALF_EXPORT EXPORT_FILE_NAME half_export.h)

target_link_libraries(ngs_half ngs_memoryallocator)

if (${ARCH} STREQUAL ARCH_WINDOWS)
elseif (${ARCH} STREQUAL ARCH_WINRT)
elseif (${ARCH} STREQUAL ARCH_ANDROID)
    target_link_libraries(ngs_half gnustl_shared)
elseif (${ARCH} STREQUAL ARCH_LINUX)
elseif (${ARCH} STREQUAL ARCH_MACOS)
endif ()



# Install.
INSTALL(TARGETS ngs_half
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT base
)

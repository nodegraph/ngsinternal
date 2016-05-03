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
	
add_library(ngs_half SHARED  ${sources} ${headers})
generate_export_header(ngs_half EXPORT_MACRO_NAME HALF_EXPORT EXPORT_FILE_NAME half_export.h)

target_link_libraries(ngs_half ngs_memoryallocator)

if (${ARCH} STREQUAL ARCH_WINDOWS)
elseif (${ARCH} STREQUAL ARCH_WINRT)
elseif (${ARCH} STREQUAL ARCH_ANDROID)
elseif (${ARCH} STREQUAL ARCH_LINUX)
elseif (${ARCH} STREQUAL ARCH_MACOS)
elseif (${ARCH} STREQUAL ARCH_IOS)
endif ()



# Install.
INSTALL(TARGETS ngs_half
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT base
)

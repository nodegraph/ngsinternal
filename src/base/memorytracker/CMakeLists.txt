set(sources
	memorytracker.cpp
	memoryops.cpp
    )
    
set(headers
    memorytracker.h
    memoryops.h
    )

add_library(ngs_memorytracker  ${sources} ${headers}) #STATIC
generate_export_header(ngs_memorytracker EXPORT_MACRO_NAME MEMORYTRACKER_EXPORT EXPORT_FILE_NAME memorytracker_export.h)


if (${ARCH} STREQUAL "ARCH_WINDOWS")
	target_link_libraries(ngs_memorytracker)
elseif (${ARCH} STREQUAL "ARCH_WINRT")
	target_link_libraries(ngs_memorytracker)
elseif (${ARCH} STREQUAL "ARCH_LINUX")
    target_link_libraries(ngs_memorytracker dl pthread)
elseif (${ARCH} STREQUAL "ARCH_MACOS")
    target_link_libraries(ngs_memorytracker dl pthread)
elseif (${ARCH} STREQUAL "ARCH_IOS")
    target_link_libraries(ngs_memorytracker dl pthread)
elseif (${ARCH} STREQUAL "ARCH_ANDROID")
	target_link_libraries(ngs_memorytracker gnustl_shared)
endif()

# Install.
INSTALL(TARGETS ngs_memorytracker
  RUNTIME DESTINATION bin
  COMPONENT base
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
)


set(sources
	bootstrap.cpp
    memoryoverrides.cpp
    memoryoverloads.cpp
    )
    
set(headers
	taggednew.h
	bootstrap.h
    memoryoverrides.h
    memoryoverloads.h	
    )

# We need to make this a static library so that it gets linked into all the dlls.
# This dynamically links to a singleton memorytracker.
# So all dlls will be allocating memory using the heap in the memorytracker dll.
# Usually each dll has its own runtime and allocates memory in its own heap.

if (${ARCH} STREQUAL "ARCH_WINDOWS")
	# Make a static lib.
	add_library(ngs_memoryallocator STATIC ${sources} ${headers})
elseif (${ARCH} STREQUAL "ARCH_WINRT")
	# Make a static lib.
	add_library(ngs_memoryallocator STATIC ${sources} ${headers})
else()
	add_library(ngs_memoryallocator ${sources} ${headers})
endif()

generate_export_header(ngs_memoryallocator EXPORT_MACRO_NAME MEMORYALLOCATOR_EXPORT EXPORT_FILE_NAME memoryallocator_export.h)

# Libraries required by all configurations.
target_link_libraries(ngs_memoryallocator ngs_memorytracker)

if (${ARCH} STREQUAL "ARCH_WINDOWS")
	target_link_libraries(ngs_memoryallocator )
elseif (${ARCH} STREQUAL "ARCH_WINRT")
	target_link_libraries(ngs_memoryallocator)
elseif (${ARCH} STREQUAL "ARCH_LINUX")
    target_link_libraries(ngs_memoryallocator dl pthread)
elseif (${ARCH} STREQUAL "ARCH_MACOS")
    target_link_libraries(ngs_memoryallocator dl pthread)
elseif (${ARCH} STREQUAL "ARCH_IOS")
    target_link_libraries(ngs_memoryallocator dl pthread)
elseif (${ARCH} STREQUAL "ARCH_ANDROID")
	target_link_libraries(ngs_memoryallocator gnustl_shared)
endif()

# Install.
INSTALL(TARGETS ngs_memoryallocator
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  COMPONENT base
)


set(sources
    testmemorytracker.cpp
)

set(headers
)

# Add the executable object.
if (${ARCH} STREQUAL ARCH_MACOS)
	add_executable(testmemorytracker MACOSX_BUNDLE ${sources} ${headers})
else()
	add_executable(testmemorytracker ${sources} ${headers})
endif()

if (${ARCH} STREQUAL "ARCH_IOS")
	set_target_properties(testmemorytracker PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()

# Link libraries.
target_link_libraries(testmemorytracker 
	ngs_memoryallocator
	${ios_libs})
    
# Install.
if (NOT (${ARCH} STREQUAL ARCH_MACOS))
	INSTALL(TARGETS testmemorytracker
	  RUNTIME DESTINATION bin
	  COMPONENT unittests
	  LIBRARY DESTINATION lib
	  ARCHIVE DESTINATION lib
	  BUNDLE DESTINATION .
	)
endif()
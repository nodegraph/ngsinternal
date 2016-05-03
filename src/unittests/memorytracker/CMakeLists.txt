set(sources
    testmemorytracker.cpp
)

set(headers
)

# Add the executable object.
add_executable(testmemorytracker ${sources} ${headers})

if (${ARCH} STREQUAL "ARCH_IOS")
	set_target_properties(testmemorytracker PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()

# Link libraries.
target_link_libraries(testmemorytracker 
	ngs_memoryallocator
	${ios_libs})
    
# Install.
INSTALL(TARGETS testmemorytracker
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  COMPONENT unittests
)
set(sources
    testnodegraph.cpp
    testplugs.cpp
    testdatanodes.cpp
)

set(headers
    testplugs.h
    testdatanodes.h
)

# Add the executable object.
add_executable(testnodegraph ${sources} ${headers})

# Link libraries.
target_link_libraries(testnodegraph 
	ngs_memoryallocator 
	ngs_entities
	${ios_libs})

if (${ARCH} STREQUAL "ARCH_IOS")
	set_target_properties(testnodegraph PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()
    
# Install.
INSTALL(TARGETS testnodegraph
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  COMPONENT unittests
)

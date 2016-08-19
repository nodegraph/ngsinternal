set(sources
    main.cpp
    testdeps.cpp
)

set(headers
    testdeps.h
)

# Add the executable object.
add_executable(testobjectmodel ${sources} ${headers})

# Link libraries.
target_link_libraries(testobjectmodel 
	ngs_memoryallocator 
	ngs_entities
	freetype-gl
	${ios_libs})
	
if (${ARCH} STREQUAL "ARCH_IOS")
	set_target_properties(testobjectmodel PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()
    
# Install.
INSTALL(TARGETS testobjectmodel
  RUNTIME DESTINATION bin
  COMPONENT unittests
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
)

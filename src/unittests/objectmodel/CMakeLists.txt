set(sources
    main.cpp
    testdeps.cpp
)

set(headers
    testdeps.h
)

# Add the executable object.
if (${ARCH} STREQUAL ARCH_MACOS)
	add_executable(testobjectmodel MACOSX_BUNDLE ${sources} ${headers})
else()
	add_executable(testobjectmodel ${sources} ${headers})
endif()

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
if (NOT (${ARCH} STREQUAL ARCH_MACOS))
	INSTALL(TARGETS testobjectmodel
	  RUNTIME DESTINATION bin
	  COMPONENT unittests
	  LIBRARY DESTINATION lib
	  ARCHIVE DESTINATION lib
	  BUNDLE DESTINATION .
	)
endif()

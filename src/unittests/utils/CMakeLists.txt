set(sources
    main.cpp
)

set(headers
)
    
# Add the executable object.
if (${ARCH} STREQUAL ARCH_MACOS)
	add_executable(testutils MACOSX_BUNDLE ${sources} ${headers})
else()
	add_executable(testutils ${sources} ${headers})
endif()

# Link libraries.
target_link_libraries(testutils 
	ngs_memoryallocator 
	ngs_utils
	${ios_libs})

if (${ARCH} STREQUAL "ARCH_IOS")
	set_target_properties(testutils PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()

# Install.
if (NOT (${ARCH} STREQUAL ARCH_MACOS))
	INSTALL(TARGETS testutils
	  RUNTIME DESTINATION bin
	  COMPONENT unittests
	  LIBRARY DESTINATION lib
	  ARCHIVE DESTINATION lib
	  BUNDLE DESTINATION .
	  )
endif()


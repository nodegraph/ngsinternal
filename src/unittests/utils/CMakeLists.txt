set(sources
    main.cpp
)

set(headers
)
    
# Add the executable object.
add_executable(testutils ${sources} ${headers})

# Link libraries.
target_link_libraries(testutils 
	ngs_memoryallocator 
	ngs_utils
	${ios_libs})

if (${ARCH} STREQUAL "ARCH_IOS")
	set_target_properties(testutils PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()

# Install.
INSTALL(TARGETS testutils
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  COMPONENT unittests
  )



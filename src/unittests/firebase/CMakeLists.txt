set(sources
    main.cpp
)

set(headers
)
    
# Add the executable object.
add_executable(testfirebase ${sources} ${headers})

# Link libraries.
target_link_libraries(testfirebase 
	ngs_memoryallocator 
	ngs_firebase
	ngs_utils
	Qt5::Core
	${ios_libs})

if (${ARCH} STREQUAL "ARCH_IOS")
	set_target_properties(testfirebase PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()

# Install.
INSTALL(TARGETS testfirebase
  RUNTIME DESTINATION bin
  COMPONENT unittests
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  )



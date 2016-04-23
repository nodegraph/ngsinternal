set(sources
    testmemorytracker.cpp
)

set(headers
)

# Add the executable object.
add_executable(testmemorytracker ${sources} ${headers})

# Link libraries.
target_link_libraries(testmemorytracker ngs_memoryallocator)
    
if(${ARCH} STREQUAL ARCH_ANDROID)
    target_link_libraries(testmemorytracker gnustl_shared)
endif()
    
# Install.
INSTALL(TARGETS testmemorytracker
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT unittests
)
set(sources
    testmemorytracker.cpp
)

set(headers
)

# Add the executable object.
add_executable(testmemorytracker ${sources} ${headers})

# Link libraries.
target_link_libraries(testmemorytracker ngs_memoryallocator)
    
# Install.
INSTALL(TARGETS testmemorytracker
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT unittests
)
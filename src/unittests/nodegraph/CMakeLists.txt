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
target_link_libraries(testnodegraph ngs_memoryallocator ngs_entities)
    
# Install.
INSTALL(TARGETS testnodegraph
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT unittests
)

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
target_link_libraries(testobjectmodel ngs_memoryallocator ngs_entities)
    
# Install.
INSTALL(TARGETS testobjectmodel
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT unittests
)

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
    
if(${ARCH} STREQUAL ARCH_ANDROID)
    target_link_libraries(testobjectmodel gnustl_shared)
endif()
    
# Install.
INSTALL(TARGETS testobjectmodel
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT unittests
)

set(sources
    main.cpp
)

set(headers
)
    
# Add the executable object.
add_executable(testutils ${sources} ${headers})

# Link libraries.
target_link_libraries(testutils ngs_memoryallocator ngs_utils )

# Install.
INSTALL(TARGETS testutils
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT unittests
  )



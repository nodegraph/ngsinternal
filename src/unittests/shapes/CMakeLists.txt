set(sources
    main.cpp
    testshapes.cpp
)

set(headers
    testshapes.h
)

# Add the executable object.
add_executable(testshapes ${sources} ${headers})
target_link_libraries(testshapes ngs_memoryallocator)

# Link libraries.
if (${ARCH} STREQUAL ARCH_LINUX)
	target_link_libraries(testshapes GL ngs_gui_glx ngs_shapes ngs_entities)
elseif ((${ARCH} STREQUAL ARCH_WINDOWS) OR (${ARCH} STREQUAL ARCH_WINRT)) 
	target_link_libraries(testshapes ngs_shapes ngs_entities ngs_gui_win)
elseif (${ARCH} STREQUAL DESKTOP_ANDROID)
endif ()
  
if(${ARCH} STREQUAL ARCH_ANDROID)
    target_link_libraries(testshapes gnustl_shared)
endif()
  
# Install.
INSTALL(TARGETS testshapes
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT unittests
)


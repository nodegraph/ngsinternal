set(sources
    main.cpp
)

set(headers
)

set(moc_headers
    clients.h
	)

qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})

# Add the executable object.
add_executable(testqmqtt ${sources} ${headers})

# Link libraries.
target_link_libraries(testqmqtt 
	ngs_memoryallocator 
	ngs_utils
	ngs_qmqtt
	${ios_libs}
	)

if (${ARCH} STREQUAL "ARCH_IOS")
	set_target_properties(testqmqtt PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()

# Install.
INSTALL(TARGETS testqmqtt
  RUNTIME DESTINATION bin
  COMPONENT unittests
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
  )



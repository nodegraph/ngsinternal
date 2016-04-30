set(sources
	main.cpp
	)
	
set(moc_headers
	)
	
qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})

if (${ARCH} STREQUAL ARCH_LINUX)
	add_executable(testguiqt ${sources})
elseif (${ARCH} STREQUAL ARCH_MACOS)
	add_executable(testguiqt ${sources})
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
	add_executable(testguiqt ${sources})
elseif (${ARCH} STREQUAL ARCH_WINRT)
	add_executable(testguiqt ${sources})
elseif (${ARCH} STREQUAL ARCH_ANDROID)
	add_library(testguiqt SHARED ${sources})
endif ()

target_link_libraries(testguiqt ngs_memoryallocator ngs_gui ngs_entities)

if(${ARCH} STREQUAL ARCH_ANDROID)
    target_link_libraries(testguiqt gnustl_shared)
endif()

INSTALL(TARGETS testguiqt
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT unitttests
)




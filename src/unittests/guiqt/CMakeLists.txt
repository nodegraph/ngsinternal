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
	add_executable(testguiqt MACOSX_BUNDLE ${sources})
elseif (${ARCH} STREQUAL ARCH_IOS)
	add_executable(testguiqt ${sources})
	set_target_properties(testguiqt PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
elseif (${ARCH} STREQUAL ARCH_WINDOWS)
	add_executable(testguiqt ${sources})
elseif (${ARCH} STREQUAL ARCH_WINRT)
	add_executable(testguiqt ${sources})
elseif (${ARCH} STREQUAL ARCH_ANDROID)
	add_library(testguiqt SHARED ${sources})
endif ()

target_link_libraries(testguiqt 
	ngs_memoryallocator 
	ngs_gui 
	ngs_entities
	${ios_libs})

if (NOT (${ARCH} STREQUAL ARCH_MACOS))
	INSTALL(TARGETS testguiqt
	  RUNTIME DESTINATION bin
	  COMPONENT unittests
	  LIBRARY DESTINATION lib
	  ARCHIVE DESTINATION lib
	  BUNDLE DESTINATION .
	)
endif()




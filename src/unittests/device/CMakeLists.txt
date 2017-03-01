set(sources
    main.cpp
    testmemory.cpp
    testrenderoutputs.cpp
    testtexture.cpp
    testtexture2.cpp
    testmultisampling.cpp
    testasyncquery.cpp
    testgeometryshader.cpp
)

set(headers
    testmemory.h
    testrenderoutputs.h
    testtexture.h
    testtexture2.h
    testmultisampling.h
    testasyncquery.h
    testgeometryshader.h
)

# Add the executable object.
if (${ARCH} STREQUAL ARCH_MACOS)
	add_executable(testdevice MACOSX_BUNDLE ${sources} ${headers})
else()
	add_executable(testdevice ${sources} ${headers})
endif()


target_link_libraries(testdevice ngs_memoryallocator ngs_device ngs_resources ngs_gui)
target_link_libraries(testdevice 
		Qt5::Core
    	Qt5::Widgets
    	Qt5::OpenGL
    	Qt5::Gui
    	Qt5::Core
    	)

# Link libraries.
if (${ARCH} STREQUAL ARCH_LINUX)
	target_link_libraries(testdevice pthread)
elseif (${ARCH} STREQUAL ARCH_WINDOWS) 
	target_link_libraries(testdevice)
elseif (${ARCH} STREQUAL ARCH_WINRT)
	target_link_libraries(testdevice)
elseif (${ARCH} STREQUAL DESKTOP_ANDROID)
elseif (${ARCH} STREQUAL ARCH_IOS)
	target_link_libraries(testdevice
		${ios_libs}
	)
	set_target_properties(testdevice PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif ()

# Install.
if (NOT (${ARCH} STREQUAL ARCH_MACOS))
	INSTALL(TARGETS testdevice
	  RUNTIME DESTINATION bin
	  COMPONENT unittests
	  LIBRARY DESTINATION lib
	  ARCHIVE DESTINATION lib
	  BUNDLE DESTINATION .
	)
endif()


    


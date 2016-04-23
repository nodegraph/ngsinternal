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
add_executable(testdevice ${sources} ${headers})
target_link_libraries(testdevice ngs_memoryallocator ngs_device ngs_resources ngs_gui_qt)
target_link_libraries(testdevice 
		debug Qt5Cored optimized Qt5Core
    	debug Qt5Widgetsd optimized Qt5Widgets
    	debug Qt5OpenGLd optimized Qt5OpenGL
    	debug Qt5Guid optimized Qt5Gui
    	debug Qt5Cored optimized Qt5Core
    	)

# Link libraries.
if (${ARCH} STREQUAL ARCH_LINUX)
	target_link_libraries(testdevice pthread)
elseif (${ARCH} STREQUAL ARCH_WINDOWS) 
	target_link_libraries(testdevice)
elseif (${ARCH} STREQUAL ARCH_WINRT)
	target_link_libraries(testdevice)
elseif (${ARCH} STREQUAL DESKTOP_ANDROID)
endif ()

if(${ARCH} STREQUAL ARCH_ANDROID)
    target_link_libraries(testdevice gnustl_shared)
endif()
    
# Install.
INSTALL(TARGETS testdevice
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT unittests
)


    



set(quick_src
    eventtoinfo.cpp
	nodegraphrenderer.cpp
	nodegraphquickitem.cpp
	nodegraphcontroller.cpp
	fborenderer.cpp
	fboworker.cpp
	texturedisplaynode.cpp
	nodegraphview.cpp
	nodegraphmanipulator.cpp
    )
   
set(quick_headers
    eventtoinfo.h
    fborenderer.h
    basenodegraphmanipulator.h
   
)
    
set(moc_quick
	nodegraphrenderer.h
	nodegraphquickitem.h
	nodegraphcontroller.h
    fboworker.h
    texturedisplaynode.h
    nodegraphview.h
    nodegraphmanipulator.h
    )
    
qt5_wrap_cpp(quick_imps ${moc_quick})
list(APPEND quick_src ${quick_imps})
    
add_library(ngs_quick  ${quick_src})
generate_export_header(ngs_quick EXPORT_MACRO_NAME QUICK_EXPORT EXPORT_FILE_NAME quick_export.h)
target_link_libraries(ngs_quick ngs_memoryallocator)
 
target_link_libraries(ngs_quick 
	ngs_objectmodel
	ngs_device
	ngs_interactions
	ngs_glewhelper
	ngs_comms
	ngs_guicomputes
	Qt5::OpenGL
   	Qt5::Core
	Qt5::Quick
	Qt5::Network
	Qt5::WebSockets
) 

if (${ARCH} STREQUAL ARCH_WINDOWS)
    target_link_libraries(ngs_quick 
        libeay32
        ssleay32
    ) 
elseif (${ARCH} STREQUAL ARCH_MACOS)
    target_link_libraries(ngs_quick 
        crypto
        ssl
    ) 
endif()

 
# Install.
INSTALL(TARGETS ngs_quick
  RUNTIME DESTINATION bin
  COMPONENT gui
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

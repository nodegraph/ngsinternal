
set(sources
    browsergroupnodecompute.cpp
    scriptgroupnodecompute.cpp
    firebasegroupnodecompute.cpp
    mqttgroupnodecompute.cpp
    scriptnodecompute.cpp
    browsercomputes.cpp
    httpcomputes.cpp
    mqttcomputes.cpp
    mqttgrouptraits.cpp
    browserrecorder.cpp
    browsergrouptraits.cpp
    firebasegrouptraits.cpp
    )
   
set(headers
    browsergroupnodecompute.h
    scriptnodecompute.h
    firebasegroupnodecompute.h
    mqttgroupnodecompute.h
    browsercomputes.h
    httpcomputes.h
    mqttcomputes.h
    mqttgrouptraits.h
    browsergrouptraits.h
    firebasegrouptraits.h
    )

    
set(moc_headers
    scriptgroupnodecompute.h
    browserrecorder.h
    )
    
qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})
    
add_library(ngs_guicomputes  ${sources})
generate_export_header(ngs_guicomputes EXPORT_MACRO_NAME GUICOMPUTES_EXPORT EXPORT_FILE_NAME guicomputes_export.h)
target_link_libraries(ngs_guicomputes ngs_memoryallocator)
 
target_link_libraries(ngs_guicomputes 
	ngs_objectmodel
	ngs_device
	ngs_interactions
	ngs_glewhelper
	ngs_comms
	ngs_computes
	Qt5::OpenGL
    Qt5::Core
	Qt5::Quick
	Qt5::Network
	Qt5::WebSockets
	libeay32
	ssleay32
) 
 
# Install.
INSTALL(TARGETS ngs_guicomputes
  RUNTIME DESTINATION bin
  COMPONENT gui
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

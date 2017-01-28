
set(sources
    baseprocess.cpp
    acceptsaveprocess.cpp
    downloadvideoprocess.cpp
    downloadmanager.cpp
    javaprocess.cpp
    messagesender.cpp
    messagereceiver.cpp
    scriptgroupnodecompute.cpp
    scriptnodecompute.cpp
    taskqueuer.cpp
    httpworker.cpp
    browsercomputes.cpp
    firebasecomputes.cpp
    httpcomputes.cpp
    mqttcomputes.cpp
    browserrecorder.cpp
    entergroupcompute.cpp
    entermqttgroupcompute.cpp
    enterfirebasegroupcompute.cpp
    enterbrowsergroupcompute.cpp
    waitnodecompute.cpp
    )
   
set(headers
    browsercomputes.h
    firebasecomputes.h
    httpcomputes.h
    mqttcomputes.h
    entergroupcompute.h
    enterfirebasegroupcompute.h
    enterbrowsergroupcompute.h
    )

    
set(moc_headers
    baseprocess.h
    acceptsaveprocess.h
    downloadvideoprocess.h
    downloadmanager.h
    javaprocess.h
    messagesender.h
    messagereceiver.h
    taskqueuer.h
    httpworker.h
    scriptgroupnodecompute.h
    browserrecorder.h
    entermqttgroupcompute.h
    scriptnodecompute.h
    waitnodecompute.h
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
) 

if (${ARCH} STREQUAL ARCH_WINDOWS)
    target_link_libraries(ngs_guicomputes 
        libeay32
        ssleay32
    ) 
elseif (${ARCH} STREQUAL ARCH_MACOS)
    target_link_libraries(ngs_guicomputes 
        crypto
        ssl
    ) 
endif()
 
# Install.
INSTALL(TARGETS ngs_guicomputes
  RUNTIME DESTINATION bin
  COMPONENT gui
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

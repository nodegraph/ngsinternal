
set(sources
    baseprocess.cpp
    acceptsaveprocess.cpp
    downloadvideoprocess.cpp
    downloadmanager.cpp
    javaprocess.cpp
    messagesender.cpp
    messagereceiver.cpp
    scriptnodecompute.cpp
    taskqueuer.cpp
    httpworker.cpp
    browsercomputes.cpp
    httpcomputes.cpp
    mqttcomputes.cpp
    browserrecorder.cpp
    mqtthostcompute.cpp
    waitnodecompute.cpp
    postnodecompute.cpp
    )
   
set(headers
    browsercomputes.h
    httpcomputes.h
    mqttcomputes.h
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
    mqtthostcompute.h
    browserrecorder.h
    scriptnodecompute.h
    waitnodecompute.h
    postnodecompute.h
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
	ngs_qmqtt
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

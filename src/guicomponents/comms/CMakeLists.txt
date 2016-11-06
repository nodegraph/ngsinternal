
set(sources
    filemodel.cpp
    message.cpp
    nodejsprocess.cpp
    messagesender.cpp
    messagereceiver.cpp
    appconfig.cpp
    taskscheduler.cpp
    browserworker.cpp
    httpworker.cpp
    browserrecorder.cpp
    licensechecker.cpp
    cryptologic.cpp
    grouptraits.cpp
    browsergrouptraits.cpp
    firebasegrouptraits.cpp
    )
   
set(headers
    taskscheduler.h
    grouptraits.h
    browsergrouptraits.h
    firebasegrouptraits.h
    cryptologic.h
)
    
set(moc_headers
    filemodel.h
    message.h
    nodejsprocess.h
    messagesender.h
    messagereceiver.h
    appconfig.h
    browserworker.h
    httpworker.h
    browserrecorder.h
    licensechecker.h
    )
    
qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})
    
add_library(ngs_comms  ${sources})
generate_export_header(ngs_comms EXPORT_MACRO_NAME COMMS_EXPORT EXPORT_FILE_NAME comms_export.h)
target_link_libraries(ngs_comms ngs_memoryallocator)
target_link_libraries(ngs_comms 
	ngs_objectmodel
	ngs_device
	ngs_interactions
	ngs_glewhelper
	#Qt5::OpenGL
    #Qt5::Core
    Qt5::Gui
	Qt5::Quick
	#Qt5::Network
	Qt5::WebSockets
	#libeay32
	#ssleay32
) 
 
# Install.
INSTALL(TARGETS ngs_comms
  RUNTIME DESTINATION bin
  COMPONENT gui
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

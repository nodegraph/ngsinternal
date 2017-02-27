
set(sources
    cryptologic.cpp
    licensechecker.cpp
    filemodel.cpp
    message.cpp
    messagetypestrings.cpp
    taskscheduler.cpp
    guitypes.cpp
    )
   
set(headers
    commtypes.h
    messagetypes.h
    messagetypestrings.h
    taskscheduler.h
)
    
set(moc_headers
    cryptologic.h
    licensechecker.h
    filemodel.h
    message.h
    guitypes.h
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
    Qt5::Gui
	Qt5::Quick
	Qt5::WebSockets
) 
 
# Install.
INSTALL(TARGETS ngs_comms
  RUNTIME DESTINATION bin
  COMPONENT gui
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

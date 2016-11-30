
set(widget_src
	actionhandler.cpp
	datasnapshot.cpp
	firebase.cpp
	json.cpp
	)
	
set(widget_headers
    json.h
    )
    
set(moc_widget
	actionhandler.h
	datasnapshot.h
	firebase.h
	)

qt5_wrap_cpp(widget_imps ${moc_widget})
list(APPEND widget_src ${widget_imps})

include_directories("${PROJECT_SOURCE_DIR}/external/firebase")

add_library(ngs_firebase  ${widget_src} )
generate_export_header(ngs_firebase EXPORT_MACRO_NAME FIREBASE_EXPORT EXPORT_FILE_NAME firebase_export.h)
target_link_libraries(ngs_firebase ngs_memoryallocator)
 
target_link_libraries(ngs_firebase 
	ngs_objectmodel
	ngs_device
	ngs_interactions
	ngs_glewhelper
	ngs_comms
	Qt5::OpenGL
    	Qt5::Core
	Qt5::Quick
	Qt5::Network
	Qt5::WebSockets
	#libeay32
	#ssleay32
) 
 
# Install.
INSTALL(TARGETS ngs_firebase
  RUNTIME DESTINATION bin
  COMPONENT gui
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

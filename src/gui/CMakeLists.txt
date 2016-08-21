
set(widget_src
	widget/testwindow.cpp
	widget/dockwidget.cpp
	widget/nodegrapheditor.cpp
	widget/splashscreen.cpp
	)
	
set(widget_headers
    )
    
set(moc_widget
	widget/testwindow.h
	widget/dockwidget.h
	widget/nodegrapheditor.h
	widget/splashscreen.h
	)

qt5_wrap_cpp(widget_imps ${moc_widget})
list(APPEND widget_src ${widget_imps})

add_library(ngs_gui  ${widget_src} )
generate_export_header(ngs_gui EXPORT_MACRO_NAME GUI_EXPORT EXPORT_FILE_NAME gui_export.h)
target_link_libraries(ngs_gui ngs_memoryallocator)
 
target_link_libraries(ngs_gui 
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
	libeay32
	ssleay32
) 
 
# Install.
INSTALL(TARGETS ngs_gui
  RUNTIME DESTINATION bin
  COMPONENT gui
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

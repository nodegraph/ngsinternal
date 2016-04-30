
set(widget_src
	widget/testwindow.cpp
	widget/dockwidget.cpp
	widget/nodegrapheditor.cpp
	widget/eventtoinfo.cpp
	widget/splashscreen.cpp
	)
	
set(quick_src
	quick/nodegraphrenderer.cpp
	quick/nodegraphquickitemglobals.cpp
	quick/nodegraphquickitem.cpp
	quick/fborenderer.cpp
	quick/fboworker.cpp
	quick/texturedisplaynode.cpp
    )
   
set(widget_headers
    widget/eventtoinfo.h
    )
    
set(quick_headers
    quick/nodegraphquickitemglobals.h    
    quick/fborenderer.h
)
    
set(moc_widget
	widget/testwindow.h
	widget/dockwidget.h
	widget/nodegrapheditor.h
	widget/splashscreen.h
	)

set(moc_quick
	quick/nodegraphrenderer.h
	quick/nodegraphquickitem.h
    quick/fboworker.h
    quick/texturedisplaynode.h
    )
    
qt5_wrap_cpp(widget_imps ${moc_widget})
list(APPEND widget_src ${widget_imps})

qt5_wrap_cpp(quick_imps ${moc_quick})
list(APPEND quick_src ${quick_imps})
    
add_library(ngs_gui  ${widget_src} ${quick_src})
generate_export_header(ngs_gui EXPORT_MACRO_NAME GUI_EXPORT EXPORT_FILE_NAME gui_export.h)
target_link_libraries(ngs_gui ngs_memoryallocator)
 
target_link_libraries(ngs_gui 
	ngs_objectmodel
	ngs_device
	ngs_interactions
	ngs_glewhelper
	Qt5::OpenGL
	Qt5::Quick
) 
 
# Install.
INSTALL(TARGETS ngs_gui
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT gui
)

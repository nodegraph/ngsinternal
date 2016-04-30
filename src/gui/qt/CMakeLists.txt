
set(sources
	testwindow.cpp
	dockwidget.cpp
	nodegrapheditor.cpp
	eventtoinfo.cpp
	splashscreen.cpp
	)
	
set(qml_sources
	qml/nodegraphrenderer.cpp
	qml/nodegraphquickitemglobals.cpp
	qml/nodegraphquickitem.cpp
	qml/fborenderer.cpp
	qml/fboworker.cpp
	qml/texturedisplaynode.cpp
    )
   
set(headers
    eventtoinfo.h
    )
    
set(qml_headers
    qml/nodegraphquickitemglobals.h    
    qml/fborenderer.h
)
    
set(moc_headers
	testwindow.h
	dockwidget.h
	nodegrapheditor.h
	splashscreen.h
	)

set(moc_qml_headers
	qml/nodegraphrenderer.h
	qml/nodegraphquickitem.h
    qml/fboworker.h
    qml/texturedisplaynode.h
    )
    
qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})

qt5_wrap_cpp(moc_qml_imps ${moc_qml_headers})
list(APPEND qml_sources ${moc_qml_imps})
    
add_library(ngs_gui_qt  ${sources} ${qml_sources})
generate_export_header(ngs_gui_qt EXPORT_MACRO_NAME GUI_QT_EXPORT EXPORT_FILE_NAME gui_qt_export.h)
target_link_libraries(ngs_gui_qt ngs_memoryallocator)
 
target_link_libraries(ngs_gui_qt 
	ngs_objectmodel
	ngs_device
	ngs_interactions
	ngs_glewhelper
	Qt5::OpenGL
	Qt5::Quick
) 
 
# Install.
INSTALL(TARGETS ngs_gui_qt
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT gui
)

# Install the headers.
#install_headers("${moc_headers}" "include/gui/qt")
#install_headers("${moc_qml_headers}" "include/gui/qt")
#install_headers("${headers}" "include/gui/qt")
#install_headers("${qml_headers}" "include/gui/qt")

# Install the export file.
#INSTALL(FILES ${CMAKE_BINARY_DIR}/gui/qt/gui_qt_export.h
#        DESTINATION "include/gui/qt")

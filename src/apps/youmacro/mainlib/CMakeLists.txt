qt5_add_resources(resources ../youmacro.qrc)

set(sources
    ${resources}
    mainlib.cpp
    )

set(headers
    mainlib.h
    )   
 
set(moc_headers
    )
    
qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})
    
add_library(ngs_mainlib  ${sources} ${headers})
generate_export_header(ngs_mainlib EXPORT_MACRO_NAME MAINLIB_EXPORT EXPORT_FILE_NAME mainlib_export.h)
target_link_libraries(ngs_mainlib 
		ngs_gui 
		ngs_entities 
	)

#qt5_use_modules(ngs_mainlib Core)

# Install.
INSTALL(TARGETS ngs_mainlib
  RUNTIME DESTINATION bin
  COMPONENT components
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)


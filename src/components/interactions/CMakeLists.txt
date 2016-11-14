
set(sources
	graphbuilder.cpp
    groupinteraction.cpp
    shapecanvas.cpp
    viewcontrols.cpp
    )
    
set(headers
	graphbuilder.h
    groupinteraction.h
    shapecanvas.h
    viewcontrols.h
    )
    
add_library(ngs_interactions  ${sources} ${headers})
generate_export_header(ngs_interactions EXPORT_MACRO_NAME INTERACTIONS_EXPORT EXPORT_FILE_NAME interactions_export.h)
target_link_libraries(ngs_interactions 
	ngs_memoryallocator 
	ngs_objectmodel 
	ngs_computes 
	ngs_compshapes 
	ngs_device)

# Install.
INSTALL(TARGETS ngs_interactions
  RUNTIME DESTINATION bin
  COMPONENT components
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)


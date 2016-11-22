
set(sources
	componentinstancer.cpp
    entityinstancer.cpp
    factory.cpp
    guientities.cpp
    nonguientities.cpp
    )
    
set(headers
	componentinstancer.h
	entityinstancer.h
	factory.h
	guientities.h
	nonguientities.h
    )
    
add_library(ngs_entities  ${sources} ${headers})
generate_export_header(ngs_entities EXPORT_MACRO_NAME ENTITIES_EXPORT EXPORT_FILE_NAME entities_export.h)
target_link_libraries(
    ngs_entities 
    ngs_memoryallocator
    ngs_objectmodel 
    ngs_computes
    ngs_compshapes
    ngs_interactions
    ngs_memoryallocator
    # guicomponents
    ngs_comms
    ngs_gui
    ngs_quick
    ngs_guicomputes
    )

# Install.
INSTALL(TARGETS ngs_entities
  RUNTIME DESTINATION bin
  COMPONENT components
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)


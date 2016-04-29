
set(sources
	componentinstancer.cpp
    entityinstancer.cpp
    factory.cpp
    guientities.cpp
    nonguientities.cpp
    )
    
set(headers
	componentids.h
	componentinstancer.h
	entityids.h
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
    ngs_memoryallocator)

#if(${ARCH} STREQUAL ARCH_ANDROID)
#    target_link_libraries(ngs_entities gnustl_shared)
#endif()

# Install.
INSTALL(TARGETS ngs_entities
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT components
)


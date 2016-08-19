
set(sources
	resources.cpp
	dataheaders/unittestshaders.cpp
)

set(headers
	resources.h
	dataheaders/unittestshaders.h
	dataheaders/displayshaders_gles2.h
	dataheaders/displayshaders_gles3.h
)

add_library(ngs_resources  ${sources} ${headers})
generate_export_header(ngs_resources EXPORT_MACRO_NAME RESOURCES_EXPORT EXPORT_FILE_NAME resources_export.h)
target_link_libraries(ngs_resources 
	ngs_memoryallocator 
	ngs_device
	ngs_utils 
	ngs_objectmodel)

INSTALL(TARGETS ngs_resources
  RUNTIME DESTINATION bin
  COMPONENT components
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/components/resources
)


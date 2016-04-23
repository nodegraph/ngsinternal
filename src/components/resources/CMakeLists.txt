
set(sources
	resources.cpp
	dataheaders/es3_shaders.cpp
)

set(headers
	resources.h
	dataheaders/es3_shaders.h
	dataheaders/monitor_shaders.h
)

add_library(ngs_resources ${sources} ${headers})
generate_export_header(ngs_resources EXPORT_MACRO_NAME RESOURCES_EXPORT EXPORT_FILE_NAME resources_export.h)
target_link_libraries(ngs_resources ngs_memoryallocator ngs_utils ngs_objectmodel)

# Install.
#add_subdirectory(fonts)
#add_subdirectory(shaders)
#add_subdirectory(qml)

# Link libraries.
target_link_libraries(ngs_resources ngs_utils ngs_device)

INSTALL(TARGETS ngs_resources
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/components/resources
  COMPONENT components
)

if(${ARCH} STREQUAL ARCH_ANDROID)
    target_link_libraries(ngs_resources gnustl_shared)
endif()

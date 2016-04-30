# ---------------------------------------------------------------------
# hub
# ---------------------------------------------------------------------

set(sources
	paramapi.cpp
	dataapi.cpp
    )
set(moc_headers
	paramapi.h
	dataapi.h
	)
set(headers
    )
    
qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})
    
add_library(ngs_api  ${sources} ${headers})
generate_export_header(ngs_api EXPORT_MACRO_NAME API_EXPORT EXPORT_FILE_NAME api_export.h)
target_link_libraries(ngs_api ngs_memoryallocator ngs_computesr)

# Install.
INSTALL(TARGETS ngs_api
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT components
)


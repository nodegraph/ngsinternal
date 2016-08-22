
set(sources
	compute.cpp
	dotnodecompute.cpp
	groupnodecompute.cpp
	inputcompute.cpp
	inputnodecompute.cpp
    mocknodecompute.cpp
    outputcompute.cpp
    outputnodecompute.cpp
    )
    
set(moc_headers
	inputcompute.h
    )
    
qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})
    
set(headers
	compute.h
	dotnodecompute.h
	groupnodecompute.h
	inputnodecompute.h
	mocknodecompute.h
	outputcompute.h
	outputnodecompute.h
    )
    
add_library(ngs_computes  ${sources} ${headers})
generate_export_header(ngs_computes EXPORT_MACRO_NAME COMPUTES_EXPORT EXPORT_FILE_NAME computes_export.h)
target_link_libraries(ngs_computes 
	ngs_memoryallocator 
	ngs_objectmodel 
	ngs_memoryallocator
	)

qt5_use_modules(ngs_computes Core)

# Install.
INSTALL(TARGETS ngs_computes
  RUNTIME DESTINATION bin
  COMPONENT components
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)


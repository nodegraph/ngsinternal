
set(sources
	compute.cpp
	computeglobals.cpp
	dotnodecompute.cpp
	groupnodecompute.cpp
	inputcompute.cpp
	inputnodecompute.cpp
    mocknodecompute.cpp
    outputcompute.cpp
    outputnodecompute.cpp
    scriptnodecompute.cpp
    )
    
set(headers
	compute.h
	computeglobals.h
	dotnodecompute.h
	groupnodecompute.h
	inputcompute.h
	inputnodecompute.h
	mocknodecompute.h
	outputcompute.h
	outputnodecompute.h
    scriptnodecompute.h
    )
    
add_library(ngs_computes  ${sources} ${headers})
generate_export_header(ngs_computes EXPORT_MACRO_NAME COMPUTES_EXPORT EXPORT_FILE_NAME computes_export.h)
target_link_libraries(ngs_computes 
	ngs_memoryallocator 
	ngs_objectmodel 
	ngs_memoryallocator
	Qt5::Core)

qt5_use_modules(ngs_computes Core)

# Install.
INSTALL(TARGETS ngs_computes
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT components
)


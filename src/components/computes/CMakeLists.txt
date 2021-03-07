
set(sources
	flux.cpp
	inputs.cpp
	outputs.cpp
	compute.cpp
	errornodecompute.cpp
	mergenodecompute.cpp
	dotnodecompute.cpp
	groupnodecompute.cpp
	ifgroupnodecompute.cpp
	whilegroupnodecompute.cpp
	inputcompute.cpp
	datanodecompute.cpp
	copydatanodecompute.cpp
	erasedatanodecompute.cpp
	inputnodecompute.cpp
    mocknodecompute.cpp
    outputcompute.cpp
    outputnodecompute.cpp
    jsonutils.cpp
    scriptloopcontext.cpp
    )
    
set(moc_headers
    )
    
qt5_wrap_cpp(moc_imps ${moc_headers})
list(APPEND sources ${moc_imps})
    
set(headers
    flux.h
    inputs.h
    outputs.h
    compute.h
    errornodecompute.h
    mergenodecompute.h
	dotnodecompute.h
	groupnodecompute.h
	ifgroupnodecompute.h
	whilegroupnodecompute.h
	inputcompute.h
	datanodecompute.h
	copydatanodecompute.h
	erasedatanodecompute.h
	inputnodecompute.h
	mocknodecompute.h
	outputcompute.h
	outputnodecompute.h
	jsonutils.h
	scriptloopcontext.h
    )
    
add_library(ngs_computes  ${sources} ${headers})
generate_export_header(ngs_computes EXPORT_MACRO_NAME COMPUTES_EXPORT EXPORT_FILE_NAME computes_export.h)
target_link_libraries(ngs_computes 
	ngs_memoryallocator 
	ngs_objectmodel 
	ngs_memoryallocator
	ngs_compshapes
	Qt5::Quick
	)

qt5_use_modules(ngs_computes Core)

# Install.
INSTALL(TARGETS ngs_computes
  RUNTIME DESTINATION bin
  COMPONENT components
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)


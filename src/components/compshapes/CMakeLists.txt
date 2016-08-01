
set(sources
    compshape.cpp
    compshapecollective.cpp
    dotnodeshape.cpp
    groupnodeshape.cpp
    inputlabelshape.cpp
    inputnodeshape.cpp
    inputshape.cpp
    linkshape.cpp
    nodegraphselection.cpp
    nodeshape.cpp
    outputlabelshape.cpp
    outputnodeshape.cpp
    outputshape.cpp
    )
    
set(headers
    compshape.h
    compshapecollective.h
    dotnodeshape.h
    groupnodeshape.h
    hittypes.h
    inputlabelshape.h
    inputnodeshape.h
    inputshape.h
    linkshape.h
    linkableshape.h
    nodeshape.h
    nodegraphselection.h
    outputlabelshape.h
	outputnodeshape.h
    outputshape.h
    )
    
add_library(ngs_compshapes  ${sources} ${headers})
generate_export_header(ngs_compshapes EXPORT_MACRO_NAME COMPSHAPES_EXPORT EXPORT_FILE_NAME compshapes_export.h)
target_link_libraries(ngs_compshapes 
	ngs_memoryallocator 
	ngs_objectmodel 
	ngs_device 
	ngs_resources 
	ngs_computes)

# Install.
INSTALL(TARGETS ngs_compshapes
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT components
)

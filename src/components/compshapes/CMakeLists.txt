
set(sources
	topology.cpp
    compshape.cpp
    compshapecollective.cpp
    dotnodeshape.cpp
    groupnodeshape.cpp
    inputlabelshape.cpp
    inputnodeshape.cpp
    inputshape.cpp
    linkshape.cpp
    nodeshape.cpp
    nodeselection.cpp
    rectnodeshape.cpp
    outputlabelshape.cpp
    outputnodeshape.cpp
    outputshape.cpp
    selectableshape.cpp
    )
    
set(headers
	topology.h
    compshape.h
    compshapecollective.h
    dotnodeshape.h
    groupnodeshape.h
    hitregion.h
    inputlabelshape.h
    inputnodeshape.h
    inputshape.h
    linkshape.h
    nodeshape.h
    rectnodeshape.h
    nodeselection.h
    outputlabelshape.h
	outputnodeshape.h
    outputshape.h
    selectableshape.h
    )
    
add_library(ngs_compshapes  ${sources} ${headers})
generate_export_header(ngs_compshapes EXPORT_MACRO_NAME COMPSHAPES_EXPORT EXPORT_FILE_NAME compshapes_export.h)
target_link_libraries(ngs_compshapes 
	ngs_memoryallocator 
	ngs_objectmodel 
	ngs_device 
	ngs_resources)

# Install.
INSTALL(TARGETS ngs_compshapes
  RUNTIME DESTINATION bin
  COMPONENT components
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

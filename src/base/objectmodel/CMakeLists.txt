
set(sources
    componentids.cpp
    entityids.cpp
    appconfig.cpp
	basefactory.cpp
	component.cpp
	deplink.cpp
	deploader.cpp
    entity.cpp
    patheddeps.cpp
    )
    
set(headers
    componentids.h
    entityids.h
    appconfig.h
	basefactory.h
	component.h
	deplink.h
	deploader.h
	entity.h
    )
    
add_library(ngs_objectmodel  ${sources} ${headers})
generate_export_header(ngs_objectmodel EXPORT_MACRO_NAME OBJECTMODEL_EXPORT EXPORT_FILE_NAME objectmodel_export.h)

target_link_libraries(ngs_objectmodel 
    ngs_memoryallocator 
    ngs_utils
    Qt5::Core
    )

if (${ARCH} STREQUAL "ARCH_WINDOWS")
    target_link_libraries(ngs_objectmodel)
elseif (${ARCH} STREQUAL "ARCH_WINRT")
	target_link_libraries(ngs_objectmodel)
elseif (${ARCH} STREQUAL "ARCH_MACOS")
	target_link_libraries(ngs_objectmodel)
elseif (${ARCH} STREQUAL "ARCH_IOS")
	target_link_libraries(ngs_objectmodel)
elseif (${ARCH} STREQUAL "ARCH_LINUX")
    target_link_libraries(ngs_objectmodel )
elseif (${ARCH} STREQUAL "ARCH_ANDROID")
	target_link_libraries(ngs_objectmodel)
endif()


# Install.
INSTALL(TARGETS ngs_objectmodel
  RUNTIME DESTINATION bin
  COMPONENT base
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
)


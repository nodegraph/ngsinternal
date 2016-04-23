# ---------------------------------------------------------------------
# utl 
# ---------------------------------------------------------------------
set(sources
	bits.cpp
    blockarray.cpp
    drawutil.cpp
    envutil.cpp
    fileutil.cpp
	functionobjects.cpp
	idgenerator.cpp
	path.cpp
	polygon.cpp
	signal.cpp
	simpleloader.cpp
	simplesaver.cpp
	stringutil.cpp
	)
set(headers
	bits.h
	blockarray.h
	drawutil.h
	envutil.h
    signal.h
    fileutil.h
	functionobjects.h
	idgenerator.h
	path.h
	permit.h
	polygon.h
	signal.h
	simpleloader.h
	simplesaver.h
	simpletypes.h
	stringutil.h
	tuplehelper.h
    )

add_library(ngs_utils ${sources} ${headers})
generate_export_header(ngs_utils EXPORT_MACRO_NAME UTILS_EXPORT EXPORT_FILE_NAME utils_export.h)

target_link_libraries(ngs_utils ngs_memoryallocator)

if (${ARCH} STREQUAL ARCH_WINDOWS)
	target_link_libraries(ngs_utils )
elseif (${ARCH} STREQUAL ARCH_WINRT)
	target_link_libraries(ngs_utils )
elseif (${ARCH} STREQUAL ARCH_ANDROID)
    target_link_libraries(ngs_utils gnustl_shared)
elseif (${ARCH} STREQUAL ARCH_LINUX)
	target_link_libraries(ngs_utils)
endif ()



# Install.
INSTALL(TARGETS ngs_utils
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT base
)



# ---------------------------------------------------------------------
# utl 
# ---------------------------------------------------------------------
set(sources
	bits.cpp
    blockarray.cpp
    crypto.cpp
    drawutil.cpp
    envutil.cpp
    fileutil.cpp
	functionobjects.cpp
	idgenerator.cpp
	path.cpp
	polyborder.cpp
	signal.cpp
	simpleloader.cpp
	simplesaver.cpp
	stringutil.cpp
	)
set(headers
	bits.h
	blockarray.h
	crypto.h
	drawutil.h
	envutil.h
    signal.h
    fileutil.h
	functionobjects.h
	idgenerator.h
	path.h
	permit.h
	polyborder.h
	signal.h
	simpleloader.h
	simplesaver.h
	simpletypes.h
	stringutil.h
	tuplehelper.h
    )

add_library(ngs_utils  ${sources} ${headers})
generate_export_header(ngs_utils EXPORT_MACRO_NAME UTILS_EXPORT EXPORT_FILE_NAME utils_export.h)

target_link_libraries(ngs_utils ngs_memoryallocator)

if (${ARCH} STREQUAL ARCH_WINDOWS)
	target_link_libraries(ngs_utils libsodium)
elseif (${ARCH} STREQUAL ARCH_MACOS)
	target_link_libraries(ngs_utils sodium)
elseif (${ARCH} STREQUAL ARCH_ANDROID)
    target_link_libraries(ngs_utils sodium)
endif ()

# Install.
INSTALL(TARGETS ngs_utils
  RUNTIME DESTINATION bin
  COMPONENT base
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  BUNDLE DESTINATION .
)



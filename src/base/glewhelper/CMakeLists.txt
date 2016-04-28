
set(sources
    glewhelper.cpp
    )

set(module_h
    glewhelper.h
    )


# Accumulate the sources and headers.
list(APPEND sources)
list(APPEND headers)

# Create a library.
add_library(ngs_glewhelper ${sources} ${headers})
generate_export_header(ngs_glewhelper EXPORT_MACRO_NAME GLEWHELPER_EXPORT EXPORT_FILE_NAME glewhelper_export.h)

target_link_libraries(ngs_glewhelper ngs_memoryallocator)

# ARCH specific.
# The glewhelper is not needed for android and winrt.

if (${ARCH} MATCHES "ARCH_WINDOWS")
	target_link_libraries(ngs_glewhelper 
		debug glew32mxd optimized glew32mx 
		debug opengl32 optimized opengl32
		debug glu32 optimized glu32)
elseif (${ARCH} MATCHES "ARCH_LINUX")
    target_link_libraries(ngs_glewhelper ngs_utils ngs_half GLEWmx GL GLU)
elseif (${ARCH} MATCHES "ARCH_MACOS")
    target_link_libraries(ngs_glewhelper ngs_utils ngs_half GLEWmx GL GLU)
elseif (${ARCH} MATCHES "ARCH_ANDROID")
	target_link_libraries(ngs_glewhelper log)
	if (gles_major_version EQUAL 2) 
		target_link_libraries(ngs_glewhelper 
			debug GLESv2 optimized GLESv2)
	elseif (gles_major_version EQUAL 3)
		target_link_libraries(ngs_glewhelper
			debug GLESv3 optimized GLESv3)
	endif ()
	
endif (${ARCH} MATCHES "ARCH_WINDOWS")

# Install.
INSTALL(TARGETS ngs_glewhelper 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT base
)


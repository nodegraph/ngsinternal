
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
add_library(ngs_glewhelper  ${sources} ${headers})
generate_export_header(ngs_glewhelper EXPORT_MACRO_NAME GLEWHELPER_EXPORT EXPORT_FILE_NAME glewhelper_export.h)

target_link_libraries(ngs_glewhelper ngs_memoryallocator)

# ARCH specific.
# The glewhelper is a stub for most architectures exept the following.
# Linux and Windows without Angle.

if (${ARCH} MATCHES "ARCH_LINUX")
    target_link_libraries(ngs_glewhelper 
    	GLEWmx GL GLU)
elseif (${ARCH} MATCHES "ARCH_WINDOWS")
	if (NOT (${gles_use_angle} MATCHES "1"))
		target_link_libraries(ngs_glewhelper 
			debug glew32mxd optimized glew32mx 
			debug opengl32 optimized opengl32
			debug glu32 optimized glu32)
	endif()
elseif (${ARCH} MATCHES "ARCH_MACOS")
elseif (${ARCH} MATCHES "ARCH_ANDROID")
	target_link_libraries(ngs_glewhelper log)	
endif ()

# Install.
INSTALL(TARGETS ngs_glewhelper 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  COMPONENT base
)


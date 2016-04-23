add_subdirectory(device)

# The glewhelper is not needed for android and winrt.
if ((NOT (${ARCH} MATCHES "ARCH_ANDROID")) AND (NOT (${gles_use_angle} MATCHES "1")))
	add_subdirectory(glewhelper)
endif()

add_subdirectory(utils)
add_subdirectory(memorytracker)
add_subdirectory(memoryallocator)
add_subdirectory(objectmodel)





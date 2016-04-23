
add_subdirectory(compshapes)
add_subdirectory(computes)
add_subdirectory(entities)
add_subdirectory(interactions)
add_subdirectory(resources)

#if ((NOT ${ARCH} MATCHES "DESKTOP_NACL") AND (NOT ${ARCH} MATCHES "DESKTOP_PNACL"))
#	add_subdirectory(api)
#endif ((NOT ${ARCH} MATCHES "DESKTOP_NACL") AND (NOT ${ARCH} MATCHES "DESKTOP_PNACL"))


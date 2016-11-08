set(public_headers
    qmqtt_client.h
    qmqtt_frame.h
    qmqtt_message.h
    qmqtt_routesubscription.h
    qmqtt_routedmessage.h
    qmqtt_router.h
    qmqtt_networkinterface.h
    qmqtt_socketinterface.h
    qmqtt_timerinterface.h
    )

set(private_headers
    qmqtt_client_p.h
    qmqtt_message_p.h
    qmqtt_network_p.h
    qmqtt_socket_p.h
    qmqtt_ssl_network_p.h
    qmqtt_ssl_socket_p.h
    qmqtt_timer_p.h
    )

set(sources
    qmqtt_client_p.cpp
    qmqtt_client.cpp
    qmqtt_frame.cpp
    qmqtt_message.cpp
    qmqtt_network.cpp
    qmqtt_ssl_network.cpp
    qmqtt_routesubscription.cpp
    qmqtt_routedmessage.cpp
    qmqtt_router.cpp
    qmqtt_message_p.cpp
    qmqtt_socket.cpp
    qmqtt_ssl_socket.cpp
    qmqtt_timer.cpp
    )

qt5_wrap_cpp(moc_imps ${public_headers} ${private_headers})
list(APPEND sources ${moc_imps})

include_directories("${PROJECT_SOURCE_DIR}/external/qmqtt")
include_directories("${PROJECT_BINARY_DIR}/external/qmqtt")

#add_definitions("-DQT_NO_CAST_TO_ASCII")
#add_definitions("-DQT_NO_CAST_FROM_ASCII")

add_library(ngs_qmqtt  ${sources})
generate_export_header(ngs_qmqtt EXPORT_MACRO_NAME Q_MQTT_EXPORT EXPORT_FILE_NAME qmqtt_global.h)
target_link_libraries(ngs_qmqtt ngs_memoryallocator)
 
target_link_libraries(ngs_qmqtt 
	ngs_objectmodel
	ngs_device
	ngs_interactions
	ngs_glewhelper
	ngs_comms
	Qt5::OpenGL
    Qt5::Core
	Qt5::Quick
	Qt5::Network
	Qt5::WebSockets
	libeay32
	ssleay32
) 
 
# Install.
INSTALL(TARGETS ngs_qmqtt
  RUNTIME DESTINATION bin
  COMPONENT gui
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)


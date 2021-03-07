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


add_library(ngs_qmqtt  ${sources})
generate_export_header(ngs_qmqtt EXPORT_MACRO_NAME Q_MQTT_EXPORT EXPORT_FILE_NAME qmqtt_global.h)
target_include_directories(ngs_qmqtt PUBLIC ${PROJECT_BINARY_DIR}/external/qmqtt)


target_link_libraries(ngs_qmqtt ngs_memoryallocator)
 
target_link_libraries(ngs_qmqtt 
    Qt5::Core
    Qt5::Network
    Qt5::WebSockets
) 

if (${ARCH} STREQUAL ARCH_WINDOWS)
    target_link_libraries(ngs_qmqtt 
        libeay32
        ssleay32
    )
elseif (${ARCH} STREQUAL ARCH_MACOS)
    target_link_libraries(ngs_qmqtt 
        crypto
        ssl
    )
elseif (${ARCH} STREQUAL ARCH_ANDROID)
    target_link_libraries(ngs_qmqtt 
        crypto
        ssl
    )
endif() 
 
# Install.
INSTALL(TARGETS ngs_qmqtt
  RUNTIME DESTINATION bin
  COMPONENT thirdparty
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)


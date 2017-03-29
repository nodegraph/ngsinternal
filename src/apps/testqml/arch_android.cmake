
# Remove the export symbols flag from the compiler flags.
# This makes all symbols get exported.
string(REPLACE "${EXPORT_SYMBOLS_FLAG}" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
string(REPLACE "${EXPORT_SYMBOLS_FLAG}" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
string(REPLACE "${EXPORT_SYMBOLS_FLAG}" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
string(REPLACE "${EXPORT_SYMBOLS_FLAG}" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})


message("android ndk root is: ${ANDROID_NDK_ROOT}")
message("android device is: ${ANDROID_DEVICE_ID}")


INSTALL(FILES 
			"${QT5_DIR}/lib/libQt5Core.so"
			"${QT5_DIR}/lib/libQt5Widgets.so"
			"${QT5_DIR}/lib/libQt5Gui.so"
			"${QT5_DIR}/lib/libQt5Quick.so"
			"${QT5_DIR}/lib/libQt5Qml.so"
			"${QT5_DIR}/lib/libQt5Network.so"
			"${QT5_DIR}/lib/libQt5OpenGL.so"
		DESTINATION lib
		COMPONENT thirdparty
		CONFIGURATIONS Debug Release)
		
		
# Setup android deployment.
find_program(ANDROID_DEPLOY_QT NAMES androiddeployqt)
if (ANDROID_DEPLOY_QT)
    message("androiddeployqt: found at: ${ANDROID_DEPLOY_QT}")
else()
    message("androiddeployqt: ERROR: NOT FOUND!")
endif()

set(QT_BINARIES_ROOT ${QT5_DIR})
set(ANDROID_SDK_BUILD_TOOLS_REVISION 23.0.3)
set(TOOLCHAIN_PREFIX arm-linux-androideabi)
set(TOOL_PREFIX arm-linux-androideabi)
set(TOOLCHAIN_VERSION 4.9)
set(NDK_HOST windows-x86_64)
set(TARGET_ARCHITECTURE armeabi-v7a)
set(QML_ROOT_PATH ${CMAKE_SOURCE_DIR}/apps/testqml/qml)
set(APPLICATION_BINARY ${CMAKE_BINARY_DIR}/install_testqml/libs/armeabi-v7a/libtestqml.so)
set(ANDROID_PACKAGE_SOURCE_DIRECTORY ${CMAKE_BINARY_DIR}/install_testqml_overrides)

# Make our main install_testqml output dir.
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/install_testqml/libs/armeabi-v7a)

# Make our install_testqml overrides dir.
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/install_testqml_overrides")

# Setup our deployment settings overrides. (Note this is done at configure time)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/packaging/android/deployment-settings.json ${ANDROID_PACKAGE_SOURCE_DIRECTORY}/deployment-settings.json @ONLY)

# ------------------------------------------------------------------------------------
# Custom command to prep the_testqml install dir.
# ------------------------------------------------------------------------------------
add_custom_command (
    DEPENDS install
    OUTPUT prep_testqml_cmd
    # copy the installed libs into the install_testqml/libs/armeabi-v7a folder
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/install/lib ${CMAKE_BINARY_DIR}/install_testqml/libs/armeabi-v7a
    # copy the gdb server file over
    COMMAND ${CMAKE_COMMAND} -E copy ${ANDROID_NDK_ROOT}/prebuilt/android-arm/gdbserver/gdbserver ${CMAKE_BINARY_DIR}/install_testqml/libs/armeabi-v7a
    # rename gdbserver to gdbserver.so
    COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/install_testqml/libs/armeabi-v7a/gdbserver ${CMAKE_BINARY_DIR}/install_testqml/libs/armeabi-v7a/gdbserver.so
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/install_testqml/libs/armeabi-v7a/gdbserver
    # Copy our android manifest override at build time.
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/packaging/android/AndroidManifest.xml ${ANDROID_PACKAGE_SOURCE_DIRECTORY}
    # Copy our java classes tree at build time.
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ANDROID_PACKAGE_SOURCE_DIRECTORY}/src
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/packaging/android/src ${ANDROID_PACKAGE_SOURCE_DIRECTORY}/src
    # Copy our resources at build time.
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ANDROID_PACKAGE_SOURCE_DIRECTORY}/res
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/packaging/android/res ${ANDROID_PACKAGE_SOURCE_DIRECTORY}/res
)

add_custom_target (prep_testqml
   DEPENDS prep_testqml_cmd
)

# ------------------------------------------------------------------------------------
# Custom command to create the debug apk.
# ------------------------------------------------------------------------------------
# Note that JAVA_HOME must be set in the environment for this to work.
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_custom_command (
        DEPENDS prep_testqml
        OUTPUT install_testqml_cmd
        COMMAND ${ANDROID_DEPLOY_QT} 
            #--verbose
            --debug
            --output "${CMAKE_BINARY_DIR}/install_testqml"
            --input "${ANDROID_PACKAGE_SOURCE_DIRECTORY}/deployment-settings.json" 
            --android-platform android-23 
            --deployment bundled 
            --ant "${PLATFORM_ROOT}/windowsunpack/apache-ant-1.9.6/bin/ant.bat"
            --device ${ANDROID_DEVICE_ID}
    )
else()
    add_custom_command (
        DEPENDS prep_testqml
        OUTPUT install_testqml_cmd
        COMMAND ${ANDROID_DEPLOY_QT}
            #--verbose
            --release
            --output "${CMAKE_BINARY_DIR}/install_testqml"
            --input "${ANDROID_PACKAGE_SOURCE_DIRECTORY}/deployment-settings.json"
            --android-platform android-23
            --deployment bundled
            --ant "${PLATFORM_ROOT}/windowsunpack/apache-ant-1.9.6/bin/ant.bat"
            --device ${ANDROID_DEVICE_ID}
            --storepass  ${PASSWORD}
            --sign ${PLATFORM_ROOT}/srcdeps/publishing/android/android_release.keystore ngsalias
    )
endif()

add_custom_target (install_testqml
   DEPENDS install_testqml_cmd
)

# ------------------------------------------------------------------------------------
# Custom command to load the debug apk onto the device.
# ------------------------------------------------------------------------------------
# Note that JAVA_HOME must be set in the environment for this to work.
# --no-build
# --reinstall
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_custom_command (
        DEPENDS prep_testqml
        OUTPUT run_testqml_cmd
        COMMAND adb uninstall com.robodownloader.testqml
        COMMAND ${ANDROID_DEPLOY_QT} 
            #--verbose
            --debug
            --reinstall
            --output "${CMAKE_BINARY_DIR}/install_testqml"
            --input "${ANDROID_PACKAGE_SOURCE_DIRECTORY}/deployment-settings.json"
            --android-platform android-23 
            --deployment bundled 
            --ant "${PLATFORM_ROOT}/windowsunpack/apache-ant-1.9.6/bin/ant.bat"
            --device ${ANDROID_DEVICE_ID}
    )
else()
    add_custom_command (
        DEPENDS prep_testqml
        OUTPUT run_testqml_cmd
        COMMAND adb uninstall com.robodownloader.testqml
        COMMAND ${ANDROID_DEPLOY_QT}
 	        --reinstall
            #--verbose
            --release
            --output "${CMAKE_BINARY_DIR}/install_testqml"
            --input "${ANDROID_PACKAGE_SOURCE_DIRECTORY}/deployment-settings.json"
            --android-platform android-23
            --deployment bundled
            --ant "${PLATFORM_ROOT}/windowsunpack/apache-ant-1.9.6/bin/ant.bat"
            --device ${ANDROID_DEVICE_ID}
            --storepass  ${PASSWORD}
            --sign ${PLATFORM_ROOT}/srcdeps/publishing/android/android_release.keystore ngsalias
    )
endif()

add_custom_target (run_testqml
   DEPENDS run_testqml_cmd
)


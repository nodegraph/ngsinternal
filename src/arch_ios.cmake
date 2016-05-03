
# ---------------------------------------------------------
# OpenGL Settings.
# --------------------------------------------------------

# GLES_MAJOR_VERSION AND GLES_MINOR_VERSION define which version of opengl es to use.
# When the GLES_MAJOR_VERSION is greater than 100, it is assumed to mean desktop opengl 4.0 and greater.

set(gles_major_version 3)
set(gles_minor_version 0)
set(gles_use_angle 0)

add_definitions("-DGLES_MAJOR_VERSION=${gles_major_version}")
add_definitions("-DGLES_MINOR_VERSION=${gles_minor_version}")
add_definitions("-DGLES_USE_ANGLE=${gles_use_angle}")

# ---------------------------------------------------------
# Qt Directory.
# ---------------------------------------------------------

set(QT5_DIR "/Users/raindrop/installs/iosandroid/Qt5.6.0/5.6/ios")


#set(OUR_LINK_FLAGS " -L${sysroot}/usr/lib -force_load  ${QT5_DIR}/plugins/platforms/libqios.a ")
#set(CMAKE_EXE_LINKER_FLAGS "${OUR_LINK_FLAGS} " CACHE STRING "" FORCE) 
#set(CMAKE_MODULE_LINKER_FLAGS "${OUR_LINK_FLAGS}" CACHE STRING "" FORCE)
#set(CMAKE_SHARED_LINKER_FLAGS "${OUR_LINK_FLAGS}" CACHE STRING "" FORCE)


# ---------------------------------------------------------
# Custom Directories. 
# ---------------------------------------------------------

set(FREETYPE_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/ios/freetype-2.5.2")
set(GLM_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/ios/glm-0.9.5.4")
set(FREETYPE_GL_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/ios/freetype-gl")

# ---------------------------------------------------------
# Our build setup for ios.
# ---------------------------------------------------------

set(BUILD_SHARED_LIBS CACHE BOOL FALSE FORCE)
set(BUILD_STATIC_LIBS CACHE BOOL TRUE FORCE)

# Enable C++11 features. (-std=gnu++0x)  -std=c++11 
# Hide symbols unless explicity exported from libraries. (fvisibility=hidden)
# Make sure there are no undefined symbols left when linking to create a shared library.   

# Debug settings.
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fvisibility=hidden -fPIC -std=gnu++11 ") # CACHE STRING "Debug options." FORCE)
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g -fvisibility=hidden -fPIC"              ) # CACHE STRING "Debug options." FORCE)

# Release settings.
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fvisibility=hidden -fPIC -std=gnu++11 ") # CACHE STRING " options." FORCE)
set(CMAKE_C_FLAGS_RELEASE   "-O3 -fvisibility=hidden -fPIC"              ) # CACHE STRING "Debug options." FORCE)

# The offline installer of Qt has some rpath issue in macos.
# You may need to type this into a shell to fix it.
# install_name_tool -add_rpath <install_loc>/Qt5.6.0/5.6/clang_64/lib <intall_loc>/Qt5.6.0/5.6/clang_64/lib/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess

# Setup up our cpp flags.
include_directories("${FREETYPE_DIR}/${IOS_SDK_ARCH}/include")
include_directories("${GLM_DIR}")
include_directories("${FREETYPE_GL_DIR}/include")

# Setup up our link flags.
link_directories("${QT5_DIR}/lib")
link_directories("${QT5_DIR}/plugins/platforms")
link_directories("${QT5_DIR}/plugins/qmltooling")
link_directories("${FREETYPE_DIR}/${IOS_SDK_ARCH}/lib")
link_directories("${FREETYPE_GL_DIR}/lib/${IOS_SDK_ARCH}")

find_library(SystemConfiguration SystemConfiguration)
find_library(CoreFoundation CoreFoundation)
find_library(OpenGLES OpenGLES)
find_library(Foundation Foundation)
find_library(Security Security)
find_library(MobileCoreServices MobileCoreServices)
find_library(QuartzCore QuartzCore)
find_library(ImageIO ImageIO)
find_library(IOKit IOKit)
find_library(CoreGraphics CoreGraphics)
find_library(WebCore WebCore)
find_library(CoreText CoreText)
find_library(MobileCoreServices MobileCoreServices)
find_library(QuartzCore QuartzCore)
find_library(UIKit UIKit)
find_library(AssetsLibrary AssetsLibrary)


# GL setup.
if (${IOS_SDK_ARCH} STREQUAL "iphoneos_armv7")
	include_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/System/Library/Frameworks/OpenGLES.framework/Headers")
	link_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/System/Library/Frameworks/OpenGLES.framework")
	link_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/usr/lib")
	set(gl_libs OpenGLES)
elseif (${IOS_SDK_ARCH} STREQUAL "iphoneos_arm64")
	include_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/System/Library/Frameworks/OpenGLES.framework/Headers")
	link_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/System/Library/Frameworks/OpenGLES.framework")
	link_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/usr/lib")
	set(gl_libs OpenGLES)
elseif (${IOS_SDK_ARCH} STREQUAL "iphonesimulator_i386")
	include_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk/System/Library/Frameworks/OpenGLES.framework/Headers")
	link_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk/System/Library/Frameworks/OpenGLES.framework")
	link_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk/usr/lib")
	set(gl_libs GLProgrammability)
elseif (${IOS_SDK_ARCH} STREQUAL "iphonesimulator_x86_64")
	include_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk/System/Library/Frameworks/OpenGLES.framework/Headers")
	link_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk/System/Library/Frameworks/OpenGLES.framework")
	link_directories("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk/usr/lib")
	set(gl_libs GLProgrammability)
endif()

set(ios_libs
		${SystemConfiguration}
		${CoreFoundation}
		${CoreGraphics}
		${CoreText}
		${QuartzCore}
		${Foundation}
		${Security}
		${UIKit}
		${AssetsLibrary}
		#
		${OpenGLES}
		${MobileCoreServices}
		#
		${ImageIO}
		${IOKit}
		#
		z
		m
		qios_debug
		Qt5PlatformSupport_debug
		Qt5DBus_debug
		qtfreetype_debug
		Qt5Widgets_debug
		qtquick2plugin_debug
		qtquickcontrolsplugin_debug
		dialogplugin_debug
		windowplugin_debug
		qmlfolderlistmodelplugin_debug
		qmlsettingsplugin_debug
		dialogsprivateplugin_debug
		qquicklayoutsplugin_debug
		qtquickextrasplugin_debug
		qtgraphicaleffectsprivate_debug
		modelsplugin_debug
		qdds_debug
		qicns_debug
		qico_debug
		qtga_debug
		qtiff_debug
		qwbmp_debug
		qwebp_debug
		qmldbg_debugger_debug
		qmldbg_inspector_debug
		Qt5Quick_debug
		qmldbg_local_debug
		qmldbg_native_debug
		Qt5Gui_debug
		qtharfbuzzng_debug
		qmldbg_profiler_debug
		qmldbg_server_debug
		qmldbg_tcp_debug
		Qt5Qml_debug
		qgenericbearer_debug
		Qt5Network_debug
		Qt5Core_debug
		qtpcre_debug
		
		
		qtlabscalendarplugin_debug
		qtlabscontrolsplugin_debug
		qtlabsmaterialstyleplugin_debug
		qtlabsuniversalstyleplugin_debug
		qmlfolderlistmodelplugin_debug
		qmlsettingsplugin_debug
		qtlabstemplatesplugin_debug
		quick3dcoreplugin_debug
		quick3dinputplugin_debug
		quick3dlogicplugin_debug
		quick3drenderplugin_debug
		declarative_audioengine_debug
		declarative_bluetooth_debug
		qtcanvas3d_debug
		qtgraphicaleffectsprivate_debug
		declarative_location_debug
		declarative_multimedia_debug
		declarative_nfc_debug
		declarative_positioning_debug
		modelsplugin_debug
		qtqmlstatemachine_debug
		qtquickcontrolsplugin_debug
		qtquickextrasflatplugin_debug
		dialogplugin_debug
		dialogsprivateplugin_debug
		qtquickextrasplugin_debug
		qquicklayoutsplugin_debug
		qmllocalstorageplugin_debug
		particlesplugin_debug
		widgetsplugin_debug
		qtquickscene3dplugin_debug
		windowplugin_debug
		qmlxmllistmodelplugin_debug
		qtquick2plugin_debug
		declarative_sensors_debug
		qmltestplugin_debug
		declarative_webchannel_debug
		declarative_qmlwebsockets_debug
		declarative_webview_debug
		
		)
link_directories(
	${QT5_DIR}/lib
	${QT5_DIR}/plugins/audio
	${QT5_DIR}/plugins/bearer
	${QT5_DIR}/plugins/generic
	${QT5_DIR}/plugins/geoservices
	${QT5_DIR}/plugins/iconengines
	${QT5_DIR}/plugins/imageformats
	${QT5_DIR}/plugins/mediaservice
	${QT5_DIR}/plugins/platforms
	${QT5_DIR}/plugins/playlistformats
	${QT5_DIR}/plugins/position
	${QT5_DIR}/plugins/qmltooling
	${QT5_DIR}/plugins/sceneparsers
	${QT5_DIR}/plugins/sensorgestures
	${QT5_DIR}/plugins/sensors
	${QT5_DIR}/plugins/sqldrivers
	${QT5_DIR}/qml/Qt/labs/calendar
	${QT5_DIR}/qml/Qt/labs/controls
	${QT5_DIR}/qml/Qt/labs/controls/material
	${QT5_DIR}/qml/Qt/labs/controls/universal
	${QT5_DIR}/qml/Qt/labs/folderlistmodel
	${QT5_DIR}/qml/Qt/labs/settings
	${QT5_DIR}/qml/Qt/labs/templates
	${QT5_DIR}/qml/Qt3D/Core
	${QT5_DIR}/qml/Qt3D/Input
	${QT5_DIR}/qml/Qt3D/Logic
	${QT5_DIR}/qml/Qt3D/Render
	${QT5_DIR}/qml/QtAudioEngine
	${QT5_DIR}/qml/QtBluetooth
	${QT5_DIR}/qml/QtCanvas3D
	${QT5_DIR}/qml/QtGraphicalEffects/private
	${QT5_DIR}/qml/QtLocation
	${QT5_DIR}/qml/QtMultimedia
	${QT5_DIR}/qml/QtNfc
	${QT5_DIR}/qml/QtPositioning
	${QT5_DIR}/qml/QtQml/Models.2
	${QT5_DIR}/qml/QtQml/StateMachine
	${QT5_DIR}/qml/QtQuick/Controls
	${QT5_DIR}/qml/QtQuick/Controls/Styles/Flat
	${QT5_DIR}/qml/QtQuick/Dialogs
	${QT5_DIR}/qml/QtQuick/Dialogs/Private
	${QT5_DIR}/qml/QtQuick/Extras
	${QT5_DIR}/qml/QtQuick/Layouts
	${QT5_DIR}/qml/QtQuick/LocalStorage
	${QT5_DIR}/qml/QtQuick/Particles.2
	${QT5_DIR}/qml/QtQuick/PrivateWidgets
	${QT5_DIR}/qml/QtQuick/Scene3D
	${QT5_DIR}/qml/QtQuick/Window.2
	${QT5_DIR}/qml/QtQuick/XmlListModel
	${QT5_DIR}/qml/QtQuick.2
	${QT5_DIR}/qml/QtSensors
	${QT5_DIR}/qml/QtTest
	${QT5_DIR}/qml/QtWebChannel
	${QT5_DIR}/qml/QtWebSockets
	${QT5_DIR}/qml/QtWebView
)



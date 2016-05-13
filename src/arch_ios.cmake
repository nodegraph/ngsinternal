
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

if (${ARCH_SIM} STREQUAL "device")
	set(QT5_DIR "/Users/raindrop/installs/iosandroid/Qt5.6.0/5.6/ios")
elseif (${ARCH_SIM} STREQUAL "simulator")
	set(QT5_DIR "/Users/raindrop/installs/iosandroid/Qt5.6.0/5.6/clang_64")
endif()

# ---------------------------------------------------------
# Custom Directories. 
# ---------------------------------------------------------

set(FREETYPE_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/ios/freetype-2.5.2")
set(GLM_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/ios/glm-0.9.5.4")
set(FREETYPE_GL_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/ios/freetype-gl")
set(LIBSODIUM_DIR "${PLATFORM_ROOT}/srcdeps/ngsexternal/ios/libsodium-1.0.10")

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
include_directories("${LIBSODIUM_DIR}/include")

# Setup up our link flags.
link_directories("${QT5_DIR}/lib")
link_directories("${QT5_DIR}/plugins/platforms")
link_directories("${QT5_DIR}/plugins/qmltooling")
link_directories("${FREETYPE_DIR}/universal")
link_directories("${FREETYPE_GL_DIR}/lib/universal")
link_directories("${LIBSODIUM_DIR}/lib")

# Find IOS frameworks.
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
find_library(OpenGLES OpenGLES)

message("OpenGLES framework is at: ${OpenGLES}")

# GL setup.
if (${ARCH_SIM} STREQUAL "device")
	include_directories("${OpenGLES}/Headers")
elseif (${ARCH_SIM} STREQUAL "simulator")
	include_directories("${OpenGLES}/Headers")
endif()

if (${ARCH_SIM} STREQUAL "simulator")
	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set(suffix _iphonesimulator_debug)
	else()
		set(suffix _iphonesimulator)
	endif()
else()
	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set(suffix _debug)
	else()
		set(suffix )
	endif()
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
		Qt5::DBus
		Qt5::Widgets
		Qt5::Quick
		Qt5::Gui
		Qt5::Qml
		Qt5::Network
		Qt5::Core
		Qt5::WebView
		#
		z
		m
		Qt5PlatformSupport${suffix}
		qios${suffix}
		qtfreetype${suffix}
		qtquick2plugin${suffix}
		qtquickcontrolsplugin${suffix}
		dialogplugin${suffix}
		windowplugin${suffix}
		qmlfolderlistmodelplugin${suffix}
		qmlsettingsplugin${suffix}
		dialogsprivateplugin${suffix}
		qquicklayoutsplugin${suffix}
		qtquickextrasplugin${suffix}
		qtgraphicaleffectsprivate${suffix}
		modelsplugin${suffix}
		qdds${suffix}
		qicns${suffix}
		qico${suffix}
		qtga${suffix}
		qtiff${suffix}
		qwbmp${suffix}
		qwebp${suffix}
		qmldbg_debugger${suffix}
		qmldbg_inspector${suffix}
		qmldbg_local${suffix}
		qmldbg_native${suffix}
		qtharfbuzzng${suffix}
		qmldbg_profiler${suffix}
		qmldbg_server${suffix}
		qmldbg_tcp${suffix}
		qgenericbearer${suffix}
		qtpcre${suffix}
		#
		qtlabscalendarplugin${suffix}
		qtlabscontrolsplugin${suffix}
		qtlabsmaterialstyleplugin${suffix}
		qtlabsuniversalstyleplugin${suffix}
		qmlfolderlistmodelplugin${suffix}
		qmlsettingsplugin${suffix}
		qtlabstemplatesplugin${suffix}
		quick3dcoreplugin${suffix}
		quick3dinputplugin${suffix}
		quick3dlogicplugin${suffix}
		quick3drenderplugin${suffix}
		declarative_audioengine${suffix}
		declarative_bluetooth${suffix}
		qtcanvas3d${suffix}
		qtgraphicaleffectsprivate${suffix}
		declarative_location${suffix}
		declarative_multimedia${suffix}
		declarative_nfc${suffix}
		declarative_positioning${suffix}
		modelsplugin${suffix}
		qtqmlstatemachine${suffix}
		qtquickcontrolsplugin${suffix}
		qtquickextrasflatplugin${suffix}
		dialogplugin${suffix}
		dialogsprivateplugin${suffix}
		qtquickextrasplugin${suffix}
		qquicklayoutsplugin${suffix}
		qmllocalstorageplugin${suffix}
		particlesplugin${suffix}
		widgetsplugin${suffix}
		qtquickscene3dplugin${suffix}
		windowplugin${suffix}
		qmlxmllistmodelplugin${suffix}
		qtquick2plugin${suffix}
		declarative_sensors${suffix}
		qmltestplugin${suffix}
		declarative_webchannel${suffix}
		declarative_qmlwebsockets${suffix}
		declarative_webview${suffix}
		
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



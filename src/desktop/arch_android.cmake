# ------------------------------------------------------------------
# Qt Plugins.
# ------------------------------------------------------------------

# Note each of these directories hold debug, release and .pdb files which gets quite big.
# We currently only weed out the pdb files from installation.
INSTALL(
  DIRECTORY 
      "${QT5_DIR}/plugins/platforms"
      "${QT5_DIR}/plugins/qmltooling"
      "${QT5_DIR}/plugins/bearer"
      "${QT5_DIR}/plugins/iconengines"
      "${QT5_DIR}/plugins/imageformats"
      #"${QT5_DIR}/plugins/platforminputcontexts"
  DESTINATION bin
  COMPONENT thirdparty
  PATTERN "*.pdb" EXCLUDE
)

# ------------------------------------------------------------------
# QML Libraries.
# ------------------------------------------------------------------
# Note each of these directories hold debug, release and .pdb files which gets quite big.
# We currently only weed out the pdb files from installation.
INSTALL(
  DIRECTORY 
      "${QT5_DIR}/qml/Qt"
      "${QT5_DIR}/qml/QtGraphicalEffects"
      "${QT5_DIR}/qml/QtQml"
      "${QT5_DIR}/qml/QtQuick"
      "${QT5_DIR}/qml/QtQuick.2"
      #"${QT5_DIR}/qml/QtWebView"
  DESTINATION bin
  COMPONENT thirdparty
  PATTERN "*.pdb" EXCLUDE
)

# ------------------------------------------------------------------
# LibSodium Libraries.
# ------------------------------------------------------------------

INSTALL(FILES 
	"${LIBSODIUM_DIR}/lib/libsodium.so"
	DESTINATION lib
	COMPONENT thirdparty
	CONFIGURATIONS Debug Release)

# ------------------------------------------------------------------
# OpenSSL Libraries.
# ------------------------------------------------------------------

INSTALL(
  FILES
      "${OPENSSL_DIR}/armeabi-v7a/libcrypto.so"
      "${OPENSSL_DIR}/armeabi-v7a/libssl.so"
  DESTINATION lib
  COMPONENT thirdparty
)

# ------------------------------------------------------------------
# Main Qt libraries.
# ------------------------------------------------------------------
		
INSTALL(FILES 
			"${QT5_DIR}/lib/libQt5Core.so"
			"${QT5_DIR}/lib/libQt5Gui.so"
			"${QT5_DIR}/lib/libQt5Network.so"
			"${QT5_DIR}/lib/libQt5OpenGL.so"
			"${QT5_DIR}/lib/libQt5Qml.so"
			"${QT5_DIR}/lib/libQt5Quick.so"
			"${QT5_DIR}/lib/libQt5Widgets.so"
			"${QT5_DIR}/lib/libQt5WebSockets.so"
			"${QT5_DIR}/lib/libQt5QuickControls2.so"
			"${QT5_DIR}/lib/libQt5QuickTemplates2.so"
		DESTINATION lib
		COMPONENT thirdparty
		CONFIGURATIONS Debug Release)
		
                    
                    

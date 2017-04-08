
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


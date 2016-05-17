#pragma once
#include <base/utils/utils_export.h>

#include <string>

namespace ngs {

class UTILS_EXPORT Crypto {
 public:

  // Generate.
  static std::string generate_nonce();
  static std::string generate_random_key();
  static std::string generate_salt();
  static std::string generate_private_key(const std::string &password, const std::string &salt);

  // The password hash is saved and used to check the password given by the user.
  static std::string generate_hashed_password(const std::string &password);
  static bool check_password(const std::string& password, const std::string& hashed_password);

  // Encrypt/Decrypt.
  static std::string encrypt(const std::string& message, const std::string& key, const std::string& nonce);
  static std::string decrypt(const std::string& cipher_text, const std::string& key, const std::string& nonce);

  // Testing.
  static void test_hashed_password(const std::string &password);
  static void test_private_key(const std::string &password);
  static void test_encrypt_decrypt(const std::string &message);

};

}

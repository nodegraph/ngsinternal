#pragma once
#include <base/utils/utils_export.h>

#include <string>

namespace ngs {

UTILS_EXPORT std::string generate_nonce();
UTILS_EXPORT std::string generate_random_key();
UTILS_EXPORT std::string generate_salt();
UTILS_EXPORT std::string generate_private_key(const std::string &password, const std::string &salt);

// Testing.
UTILS_EXPORT void test_password_encrypt_decrypt(const std::string &message, const std::string &password, const std::string& salt, const std::string& nonce);
UTILS_EXPORT void test_encrypt_decrypt(const std::string &message);

}

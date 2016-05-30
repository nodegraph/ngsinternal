#include <base/utils/crypto.h>

#include <iostream>
#include <string>
#include <cassert>

#if ARCH == ARCH_IOS
#define SODIUM_STATIC
#endif

#include <sodium.h>

namespace ngs {

// Returns the nonce.
std::string Crypto::generate_nonce() {
  std::string nonce;
  nonce.resize(crypto_secretbox_NONCEBYTES, 'a');
  randombytes_buf(&nonce[0], nonce.size());
  return nonce;
}

// Returns a random key.
std::string Crypto::generate_random_key() {
  std::string key;
  key.resize(crypto_secretbox_KEYBYTES, 'a');
  randombytes_buf(&key[0], key.size());
  return key;
}

// Returns the salt.
std::string Crypto::generate_salt() {
  std::string salt;
  salt.resize(crypto_pwhash_SALTBYTES);
  randombytes_buf((unsigned char*)&salt[0], salt.size());
  return salt;
}

// Returns the key.
std::string Crypto::generate_private_key(const std::string &password, const std::string &salt) {
  std::string key;
  key.resize(crypto_box_SEEDBYTES);
  if (crypto_pwhash((unsigned char*)&key[0], key.size(),
                    &password[0], password.size(),
                    (unsigned char*)&salt[0],
                    crypto_pwhash_OPSLIMIT_INTERACTIVE,
                    crypto_pwhash_MEMLIMIT_INTERACTIVE,
                    crypto_pwhash_ALG_DEFAULT) != 0)
  {
    // out of memory?
    assert(false);
  }
  return key;
}

std::string Crypto::generate_hashed_password(const std::string &password) {
  std::string hashed_password;
  hashed_password.resize(crypto_pwhash_STRBYTES);

  if (crypto_pwhash_str(&hashed_password[0],
                        &password[0],
                        password.size(),
                        crypto_pwhash_OPSLIMIT_INTERACTIVE,
                        crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
    /* out of memory */
    assert(false);
  }
  return hashed_password;
}

bool Crypto::check_password(const std::string& password, const std::string& hashed_password) {
  assert(hashed_password.size() == crypto_pwhash_STRBYTES);
  if (crypto_pwhash_str_verify(&hashed_password[0],
                               &password[0],
                               password.size()) != 0) {
    /* wrong password */
    return false;
  }
  return true;
}

// Returns cipher text.
std::string Crypto::encrypt(const std::string& message, const std::string& key, const std::string& nonce) {
  // Allocate space for cipher text.
  std::string cipher_text;
  size_t cipher_length = crypto_secretbox_MACBYTES + message.size();
  cipher_text.resize(cipher_length,'a');

  // Encrypt.
  if (crypto_secretbox_easy((unsigned char*)&cipher_text[0],
                            (unsigned char*)&message[0],
                            message.size(),
                            (unsigned char*)&nonce[0],
                            (unsigned char*)&key[0]) != 0) {
    assert(false);
  }
  return cipher_text;
}

// Returns original message.
std::string Crypto::decrypt(const std::string& cipher_text, const std::string& key, const std::string& nonce) {
  // Allocate space for the original message.
  std::string decrypted;

  std::cerr << "cipher text size: " << cipher_text.size() <<"\n";
  std::cerr << "key: " << key.size() <<"\n";
  std::cerr << "nonce: " << nonce.size() <<"\n";

  decrypted.resize(cipher_text.size() - crypto_secretbox_MACBYTES);
  if (crypto_secretbox_open_easy((unsigned char*)&decrypted[0],
                                 (unsigned char*)&cipher_text[0],
                                 cipher_text.size(),
                                 (unsigned char*)&nonce[0],
                                 (unsigned char*)&key[0]) != 0) {
    assert(false);
  }
  return decrypted;
}

void Crypto::test_hashed_password(const std::string &password) {
  std::string hashed_password = generate_hashed_password(password);
  assert(check_password(password, hashed_password));
  assert(!check_password(password+"123", hashed_password));
}

void Crypto::test_private_key(const std::string &password) {
  std::string salt = generate_salt();
  std::string key = generate_private_key(password, salt);
  assert(key != password);
}

void Crypto::test_encrypt_decrypt(const std::string &message) {
  std::string nonce = generate_nonce();
  std::string key = generate_random_key();

  // Encrypt.
  std::string cipher_text = encrypt(message, key, nonce);
  //std::cerr << "encrypted: " << cipher_text << "\n";

  // Decrypt.
  std::string decrypted = decrypt(cipher_text, key, nonce);
  //std::cerr << "decrypted: " << decrypted << "\n";

  // Check.
  assert(decrypted == message);
}

}


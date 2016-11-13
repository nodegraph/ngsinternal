#include <base/objectmodel/deploader.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <base/utils/bits.h>
#include <base/utils/crypto.h>

#include <ngsversion.h>

#include <components/interactions/graphbuilder.h>

#include <guicomponents/comms/appconfig.h>
#include <guicomponents/quick/nodegraphquickitem.h>

#include <entities/entityids.h>
#include <entities/guientities.h>

#include <cstddef>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <guicomponents/comms/nodejsworker.h>
#include <guicomponents/comms/cryptologic.h>

#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtQml/QtQml>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlApplicationEngine>

namespace ngs {

const QString CryptoLogic::kCryptoFile = "app_init.dat"; // Stores info about our encryption.
const QString CryptoLogic::kAppDir = "app_data";

CryptoLogic::CryptoLogic(Entity* app_root)
    : Component(app_root, kIID(), kDID()),
      _use_encryption(true) {
  // Make sure the data dir exists.
  QString data_dir = AppConfig::get_user_data_dir();
  if (!QDir(data_dir).exists()) {
    QDir().mkpath(data_dir);
  }

  // Make sure the app data dir exists.
  _app_dir = data_dir + "/" + kAppDir;
  QFileInfo info(_app_dir);
  if (!info.exists()) {
    QDir().mkpath(_app_dir);
  }

#ifdef SKIP_ENCRYPTION
  _use_encryption = false;
#endif
}

CryptoLogic::~CryptoLogic() {
}

QString CryptoLogic::get_edition() const {
  external();
  return _edition.c_str();
}

QString CryptoLogic::get_license() const {
  external();
  return _license.c_str();
}

void CryptoLogic::set_license(const QString& edition, const QString& license) {
  external();
  _edition = edition.toStdString();
  _license = license.toStdString();
}

void CryptoLogic::create_crypto(const QString& chosen_password) {
  external();
  assert(_nonce.empty());
  assert(_key.empty());

  _nonce = Crypto::generate_nonce();
  _salt = Crypto::generate_salt();
  _hashed_password = Crypto::generate_hashed_password(chosen_password.toStdString());
  _key = Crypto::generate_private_key(chosen_password.toStdString(), _salt); // The key is never saved.
}

void CryptoLogic::save_crypto() const{
  external();
  // Save out the crypto config.
  {
    std::stringstream ss;
    SimpleSaver saver(ss);
    saver.save(_nonce);
    saver.save(_salt);
    saver.save(_hashed_password);
    saver.save(_edition);
    saver.save(_license);
    _write_file(kCryptoFile, ss.str(), false);
  }
}

bool CryptoLogic::crypto_exists() const {
  external();
  if (file_exists(kCryptoFile)) {
    return true;
  }
  return false;
}

void CryptoLogic::load_crypto() {
  external();
  if (!file_exists(kCryptoFile)) {
    return;
  }

  // Load the crypto file.
  QByteArray contents = _load_file(kCryptoFile, false);
  Bits* bits = create_bits_from_raw(contents.data(), contents.size());
  SimpleLoader loader(bits);

  // Extract the nonce and salt.
  loader.load(_nonce);
  loader.load(_salt);
  loader.load(_hashed_password);
  loader.load(_edition);
  loader.load(_license);
}

bool CryptoLogic::check_password(const QString& password) {
  external();
  if (!Crypto::check_password(password.toStdString(), _hashed_password)) {
    return false;
  }
  // Generate the private key.
  _key = Crypto::generate_private_key(password.toStdString(), _salt);
  return true;
}

QString CryptoLogic::get_prefixed_file(const QString& file) const {
  external();
  return _app_dir + "/" + file;
}

// Data operations.
std::string CryptoLogic::encrypt_data(const std::string& data) const {
  external();
  return Crypto::encrypt(data, _key, _nonce);
}

std::string CryptoLogic::decrypt_data(const std::string& encrypted_data) const {
  external();
  return Crypto::decrypt(encrypted_data, _key, _nonce);
}

void CryptoLogic::_write_file(const QString& filename, const std::string& data, bool encrypt) const {
  external();
  QString full_name = get_prefixed_file(filename);
  //std::cerr << "saving to file: " << full_name.toStdString() << "\n";

  QFile file(full_name);
  file.open(QIODevice::WriteOnly);

  // Encrypt the data.
  if (encrypt) {
    std::string cipher_text = encrypt_data(data);
    file.write(cipher_text.c_str(), cipher_text.size());

    // test
    {
      QByteArray test(cipher_text.c_str(), static_cast<int>(cipher_text.size()));
      std::string test2(test.data(), test.size());
      std::string decrypted = decrypt_data(test2);
      assert(decrypted == data);
    }
  } else {
    file.write(data.c_str(), data.size());
  }
  file.close();
}

QByteArray CryptoLogic::_load_file(const QString& filename, bool decrypt) const {
  external();
  QString full_name = get_prefixed_file(filename);
  //std::cerr << "loading from file: " << full_name.toStdString() << "\n";

  // If this file doesn't exist there is nothing to load.
  if (!QFileInfo::exists(full_name)) {
    return QByteArray();
  }

  // Read all the bytes from the file.
  QFile file(full_name);
  file.open(QIODevice::ReadOnly);
  QByteArray contents = file.readAll();

  // Decrypt the data.
  if (decrypt) {
    std::string cipher_text(contents.data(), contents.size());
    std::string decrypted = decrypt_data(cipher_text);
    QByteArray contents2(&decrypted[0], static_cast<int>(decrypted.size()));
    return contents2;
  }

  return contents;
}

void CryptoLogic::destroy_file(const QString& filename) const {
  external();
  QString full_name = get_prefixed_file(filename);
  // Erase the file.
  QFile file(full_name);
  file.remove();
}

bool CryptoLogic::file_exists(const QString& filename) const {
  external();
  QString full = get_prefixed_file(filename);
  QFileInfo info(full);
  if (info.exists()) {
    return true;
  }
  return false;
}

QString CryptoLogic::make_filename_unique(const QString& filename) const {
  external();
  std::string name = filename.toStdString();
  size_t last_index = name.find_last_not_of("0123456789");
  std::string suffix = name.substr(last_index + 1);
  std::string prefix = name.substr(0,last_index+1);
  size_t number=0;
  try {
    number = boost::lexical_cast<size_t>(suffix);
  } catch (...) {
    number = 1;
  }

  bool exists = true;
  while(exists) {
    name = prefix + boost::lexical_cast<std::string>(number);
    if (!file_exists(name.c_str())) {
      return name.c_str();
    }
    number+=1;
  }
  return "error";
}

}

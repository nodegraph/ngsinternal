#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <entities/componentids.h>

#include <QtGui/QStandardItemModel>

namespace ngs {

class GraphBuilder;

class COMMS_EXPORT CryptoLogic: public Component {
 public:

  static const QString kCryptoFile;
  static const QString kAppDir;

  COMPONENT_ID(CryptoLogic, CryptoLogic)

  CryptoLogic(Entity* app_root);
  virtual ~CryptoLogic();

  // App license.
  QString get_edition() const;
  QString get_license() const;
  void set_license(const QString& edition, const QString& license);

  // Password Setup.
  void create_crypto(const QString& chosen_password);
  void save_crypto() const;
  bool crypto_exists() const;
  void load_crypto();
  bool check_password(const QString& password);

  // File save and load with or without encryption.
  void write_file(const QString& filename, const std::string& data) const {_write_file(filename,data,_use_encryption);}
  QByteArray load_file(const QString& filename) const {return _load_file(filename,_use_encryption);}

  // Other file operations.
  void destroy_file(const QString& filename) const;
  bool file_exists(const QString& filename) const;
  QString make_filename_unique(const QString& filename=QString("graph1")) const;

 private:
  QString get_prefixed_file(const QString& file) const;

  // Cipher operations.
  std::string encrypt_data(const std::string& data) const;
  std::string decrypt_data(const std::string& cipher_text) const;

  // File save and load with or without encryption.
  void _write_file(const QString& filename, const std::string& data, bool encrypt = false) const;
  QByteArray _load_file(const QString& filename, bool decrypt = false) const;


  bool _use_encryption;

  // User's app data directory where all app data is saved.
  QString _app_dir;

  // License Key for Application Usage.
  std::string _edition; // The edition of the app: currently "pro" or "lite"
  std::string _license;

  // Crypto.
  std::string _nonce;
  std::string _salt;
  std::string _hashed_password;
  std::string _key;
};

}

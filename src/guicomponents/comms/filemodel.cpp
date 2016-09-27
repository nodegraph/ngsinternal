#include <base/objectmodel/deploader.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <base/utils/bits.h>
#include <base/utils/crypto.h>

#include <ngsversion.h>

#include <components/interactions/graphbuilder.h>

#include <guicomponents/comms/appworker.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/quick/nodegraphquickitem.h>

#include <entities/entityids.h>
#include <entities/guientities.h>

#include <cstddef>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtQml/QtQml>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlApplicationEngine>

namespace ngs {

const QString FileModel::kCryptoFile = "app_init.dat"; // Stores info about our encryption.
const QString FileModel::kAppFile = "app_data.dat"; // Stores the mapping between displayed filename and actual filename.
const QString FileModel::kAppDir = "app_data";

FileModel::FileModel(Entity* app_root)
    : QStandardItemModel(),
      Component(app_root, kIID(), kDID()),
      _graph_builder(this),
      _working_row(-1),
      _use_encryption(true){
  get_dep_loader()->register_fixed_dep(_graph_builder, Path({}));

  // Make sure the data dir exists.
  QString data_dir = AppWorker::get_user_data_dir();
  if (!QDir(data_dir).exists()) {
    QDir().mkpath(data_dir);
  }

  // Make sure the app data dir exists.
  _app_dir = data_dir + "/" + kAppDir;
  QFileInfo info(_app_dir);
  if (!info.exists()) {
    QDir().mkpath(_app_dir);
  }

  // Set our role names.
  _roles[kTitleRole] = "title";
  _roles[kFilenameRole] = "filename";
  _roles[kDescriptionRole] = "description";
  _roles[kAutoRunRole] = "auto_run";
  _roles[kAutoRunIntervalRole] = "auto_run_interval";
  _roles[kHidePasswordsRole] = "hide_passwords";
  _roles[kLockLinksRole] = "lock_links";
  _roles[kMaxNodePostsRole] = "max_node_posts";
  _roles[kBrowserWidthRole] = "browser_width";
  _roles[kBrowserHeightRole] = "browser_height";
  setItemRoleNames(_roles);

  // Set up our default role values.
  _default_settings["title"] = "untitled";
  _default_settings["description"] = "An empty graph.";
  _default_settings["auto_run"] = false;
  _default_settings["auto_run_interval"] = 60;
  _default_settings["hide_passwords"] = true;
  _default_settings["lock_links"] = false;
  _default_settings["max_node_posts"] = 1000;
  _default_settings["browser_width"] = 1024;
  _default_settings["browser_height"] = 1150;

  // Sort on titles.
  setSortRole(kTitleRole);
  sort(0,Qt::AscendingOrder);

  // Connect signals.
  connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_item_changed(QStandardItem*)));
}

FileModel::~FileModel() {
}

bool FileModel::get_hide_passwords() const {
  return get_work_setting(FileModel::kHidePasswordsRole).toBool();
}

void FileModel::set_hide_passwords(bool hide) {
  set_work_setting(FileModel::kHidePasswordsRole, hide);
  emit hide_passwords_changed();
}

int FileModel::get_max_node_posts() const {
  return get_work_setting(FileModel::kMaxNodePostsRole).toInt();
}

void FileModel::set_max_node_posts(int max) {
  set_work_setting(FileModel::kMaxNodePostsRole, max);
  emit max_node_posts_changed();
}

void FileModel::on_item_changed(QStandardItem* item) {
  external();
  save_model();
}

QString FileModel::get_edition() const {
  external();
  return _edition.c_str();
}

QString FileModel::get_license() const {
  external();
  return _license.c_str();
}

void FileModel::set_license(const QString& edition, const QString& license) {
  external();
  _edition = edition.toStdString();
  _license = license.toStdString();
}

void FileModel::create_crypto(const QString& chosen_password) {
  external();
  assert(_nonce.empty());
  assert(_key.empty());

  _nonce = Crypto::generate_nonce();
  _salt = Crypto::generate_salt();
  _hashed_password = Crypto::generate_hashed_password(chosen_password.toStdString());
  _key = Crypto::generate_private_key(chosen_password.toStdString(), _salt); // The key is never saved.
}

void FileModel::save_crypto() const{
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
    write_file(kCryptoFile, ss.str());
  }
}

bool FileModel::crypto_exists() const {
  external();
  if (file_exists(kCryptoFile)) {
    return true;
  }
  return false;
}

void FileModel::load_crypto() {
  external();
  if (!file_exists(kCryptoFile)) {
    return;
  }

  // Load the crypto file.
  QByteArray contents = load_file(kCryptoFile);
  Bits* bits = create_bits_from_raw(contents.data(), contents.size());
  SimpleLoader loader(bits);

  // Extract the nonce and salt.
  loader.load(_nonce);
  loader.load(_salt);
  loader.load(_hashed_password);
  loader.load(_edition);
  loader.load(_license);
}

bool FileModel::check_password(const QString& password) {
  external();
  if (!Crypto::check_password(password.toStdString(), _hashed_password)) {
    return false;
  }
  // Generate the private key.
  _key = Crypto::generate_private_key(password.toStdString(), _salt);
  return true;
}


void FileModel::sort_files() {
  external();
  QString title = data(index(_working_row,0), kTitleRole).toString();
  sort(0,Qt::AscendingOrder);
  _working_row = find_index(title);
  assert(_working_row>=0);
}

int FileModel::find_index(const QString& title) const {
  external();
  for (int i=0; i<rowCount(); ++i) {
    if (title == data(index(i,0), kTitleRole).toString()) {
      return i;
    }
  }
  return -1;
}

int FileModel::get_working_row() const {
  external();
  return _working_row;
}

int FileModel::get_role(const QString& role_name) const {
  external();
  QHash<int, QByteArray>::const_iterator iter;
  for (iter = _roles.begin(); iter != _roles.end(); ++iter) {
    if (iter.value() == role_name.toUtf8()) {
      return iter.key();
    }
  }
  assert(false);
  return -1;
}

QVariant FileModel::get_setting(int row, int role) const {
  external();
  return data(index(row,0), role);
}

QVariant FileModel::get_setting(int row, const QString& role_name) const {
  external();
  int role = get_role(role_name);
  return get_setting(row, role);
}

void FileModel::set_setting(int row, int role, const QVariant& value) {
  external();
  if (role == kTitleRole) {
    // The title role is treated as an edge case as we need to make sure it's unique.
    QVariant current_title = data(index(row,0),kTitleRole);
    QVariant next_title = value;
    if (next_title != current_title) {
      next_title = make_title_unique(value.toString());
    }
    QStandardItem* it = item(row, 0);
    it->setData(next_title, kTitleRole);
    setItem(row,0,it);
    save_model();
  } else {
    QStandardItem* it = item(row, 0);
    it->setData(value, role);
    setItem(row,0,it);
    save_model();
  }
}

void FileModel::set_setting(int row, const QString& role_name, const QVariant& value) {
  external();
  int role = get_role(role_name);
  return set_setting(row, role, value);
}

QVariant FileModel::get_work_setting(int role) const {
  external();
  int row = get_working_row();
  return get_setting(row, role);
}

QVariant FileModel::get_work_setting(const QString& role_name) const {
  external();
  int row = get_working_row();
  return get_setting(row, role_name);
}

void FileModel::set_work_setting(int role, const QVariant& value) {
  external();
  int row = get_working_row();
  set_setting(row, role, value);
}

void FileModel::set_work_setting(const QString& role_name, const QVariant& value) {
  external();
  int row = get_working_row();
  set_setting(row, role_name, value);
}

QString FileModel::get_prefixed_file(const QString& file) const {
  external();
  return _app_dir + "/" + file;
}

// Data operations.
std::string FileModel::encrypt_data(const std::string& data) const {
  external();
  return Crypto::encrypt(data, _key, _nonce);
}

std::string FileModel::decrypt_data(const std::string& encrypted_data) const {
  external();
  return Crypto::decrypt(encrypted_data, _key, _nonce);
}

void FileModel::write_file(const QString& filename, const std::string& data, bool encrypt) const {
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
      QByteArray test(cipher_text.c_str(), cipher_text.size());
      std::string test2(test.data(), test.size());
      std::string decrypted = decrypt_data(test2);
      assert(decrypted == data);
    }
  } else {
    file.write(data.c_str(), data.size());
  }
  file.close();
}

QByteArray FileModel::load_file(const QString& filename, bool decrypt) const {
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
    QByteArray contents2(&decrypted[0], decrypted.size());
    return contents2;
  }

  return contents;
}

void FileModel::destroy_file(const QString& filename) const {
  external();
  QString full_name = get_prefixed_file(filename);
  // Erase the file.
  QFile file(full_name);
  file.remove();
}

bool FileModel::title_exists(const QString& title) const {
  external();
  for (int i=0; i<rowCount(); ++i) {
    if (title == data(index(i,0), kTitleRole).toString()) {
      return true;
    }
  }
  return false;
}

QString FileModel::make_title_unique(const QString& title) const {
  external();
  if (!title_exists(title)) {
    return title;
  }

  std::string name = title.toStdString();
  size_t last_index = name.find_last_not_of("0123456789");
  std::string suffix = name.substr(last_index + 1);
  std::string prefix = name.substr(0,last_index+1);

  // We start numbering at 1.
  size_t number=1;
  try {
    number = boost::lexical_cast<size_t>(suffix);
  } catch (...) {
    number = 1;
  }

  bool exists = true;
  while(exists) {
    name = prefix + boost::lexical_cast<std::string>(number);
    if (!title_exists(name.c_str())) {
      return name.c_str();
    }
    number+=1;
  }
  return "error";
}

bool FileModel::file_exists(const QString& filename) const {
  external();
  QString full = get_prefixed_file(filename);
  QFileInfo info(full);
  if (info.exists()) {
    return true;
  }
  return false;
}

QString FileModel::make_filename_unique(const QString& filename) const {
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

void FileModel::load_model() {
  external();
  QByteArray contents = load_file(kAppFile, true);
  if (contents.size()==0) {
    return;
  }

  // Now load them.
  Bits* bits = create_bits_from_raw(contents.data(),contents.size());
  SimpleLoader loader(bits);

  // Load the current row at the time of serialization.
  loader.load(_working_row);

  // Load the number of files.
  size_t num_rows;
  loader.load(num_rows);

  for (size_t i=0; i<num_rows; ++i) {
    std::string title;
    loader.load(title);
    std::string filename;
    loader.load(filename);
    std::string description;
    loader.load(description);
    bool auto_run;
    loader.load(auto_run);
    int auto_run_interval;
    loader.load(auto_run_interval);
    bool hide_passwords;
    loader.load(hide_passwords);
    bool lock_links;
    loader.load(lock_links);
    int max_node_posts;
    loader.load(max_node_posts);
    int browser_width;
    loader.load(browser_width);
    int browser_height;
    loader.load(browser_height);

    QStandardItem *item = new_ff QStandardItem();
    QString qtitle(title.c_str());
    item->setData(qtitle, kTitleRole);
    item->setData(filename.c_str(), kFilenameRole);
    item->setData(description.c_str(), kDescriptionRole);
    item->setData(auto_run, kAutoRunRole);
    item->setData(auto_run_interval, kAutoRunIntervalRole);
    item->setData(hide_passwords, kHidePasswordsRole);
    item->setData(lock_links, kLockLinksRole);
    item->setData(max_node_posts, kMaxNodePostsRole);
    item->setData(browser_width, kBrowserWidthRole);
    item->setData(browser_height, kBrowserHeightRole);
    setItem(i, 0, item);
  }

  assert(_working_row >= 0);

  // Update qml.
  emit hide_passwords_changed();
  emit max_node_posts_changed();
}

void FileModel::save_model() {
  external();
  // Save out our data to a string.
  std::stringstream ss;
  {
    SimpleSaver saver(ss);

    // Save the current filename that was opened.
    saver.save(_working_row);

    // Save the number of files.
    size_t row_count = rowCount();
    saver.save(row_count);

    for (size_t i=0; i<row_count; ++i) {
      saver.save(data(index(i,0), kTitleRole).toString().toStdString());
      saver.save(data(index(i,0), kFilenameRole).toString().toStdString());
      saver.save(data(index(i,0), kDescriptionRole).toString().toStdString());
      saver.save(data(index(i,0), kAutoRunRole).toBool());
      saver.save(data(index(i,0), kAutoRunIntervalRole).toInt());
      saver.save(data(index(i,0), kHidePasswordsRole).toBool());
      saver.save(data(index(i,0), kLockLinksRole).toBool());
      saver.save(data(index(i,0), kMaxNodePostsRole).toInt());
      saver.save(data(index(i,0), kBrowserWidthRole).toInt());
      saver.save(data(index(i,0), kBrowserHeightRole).toInt());
    }
  }

  // Save the data.
  write_file(kAppFile, ss.str(), _use_encryption);

  // Update qml.
  emit hide_passwords_changed();
  emit max_node_posts_changed();
}

void FileModel::load_graph() {
  external();
  if (_working_row < 0) {
    // Create the first graph file.
    QVariantMap info;
    info["title"] = "demo";
    info["description"] = "A simple demo.";
    create_graph(info);
    // Build the default graph.
    _graph_builder->build_test_graph();
    get_app_root()->initialize_wires();
    get_app_root()->clean_wires();
    save_graph();
  } else {
    load_graph(_working_row);
  }
}

void FileModel::save_graph() {
  external();
  if (_working_row < 0) {
    return;
  }
  save_graph(_working_row);
}

void FileModel::load_graph(int row) {
  std::cerr << "Starting to load graph at row: " << row << "\n";
  external();
  _working_row = row;

  // Load the graph file.
  QString graph_file = data(index(row,0), kFilenameRole).toString();
  QByteArray contents = load_file(graph_file, true);

  // Now load the data into the app root entity.
  Bits* bits = create_bits_from_raw(contents.data(),contents.size());
  SimpleLoader loader(bits);

  // Load the version number.
  size_t major;
  size_t minor;
  size_t patch;
  size_t tweak;
  loader.load(major);
  loader.load(minor);
  loader.load(patch);
  loader.load(tweak);
  //std::cerr << "version number is: " << major << "." << minor << "." << patch << "." << tweak << "\n";

  // Load off some pre data.
  size_t derived_id;
  loader.load(derived_id);
  std::string name;
  loader.load(name);

  //get_root_group()->destroy_all_children();
  get_root_group()->load(loader);

  std::cerr << "done loading graph -  aaaa\n";

  // Although everything down from the root group is updated by the load.
  // Everything from the app root is not updated. So we update it here.
  get_root_group()->initialize_wires();

  std::cerr << "done loading graph -  bbbb\n";

  get_app_root()->clean_wires();

  std::cerr << "done loading graph\n";

  // Save the model with the latest row.
  save_model();
}

void FileModel::save_graph(int row) {
  external();
  _working_row = row;

  // Save the graph to a string.
  std::stringstream ss;
  {
    SimpleSaver saver(ss);
    // Save the version number.
    size_t major = NGS_VERSION_MAJOR;
    size_t minor = NGS_VERSION_MINOR;
    size_t patch = NGS_VERSION_PATCH;
    size_t tweak = NGS_VERSION_TWEAK;

    saver.save(major);
    saver.save(minor);
    saver.save(patch);
    saver.save(tweak);
    this->get_root_group()->save(saver);
  }

  // Write the graph file.
  QString graph_file = data(index(row,0), kFilenameRole).toString();
  write_file(graph_file, ss.str(), _use_encryption);
}

// The info argument must contain all required key-value pairs.
void FileModel::create_graph(const QVariantMap& arg) {
  external();
  // Fill any missing key-value with default values.
  QVariantMap settings = _default_settings;
  QVariantMap::const_iterator iter;
  for (iter = arg.constBegin(); iter != arg.constEnd(); ++iter) {
    settings[iter.key()] = iter.value();
  }

  // Add a new row.
  setRowCount(rowCount()+1);
  _working_row = rowCount() -1;

  // Create a unique filename.
  QString filename = make_filename_unique();

  // Make sure the title is unique.
  QString title;
  title = settings["title"].toString();
  title = make_title_unique(title);

  // Build the new item.
  QStandardItem *item = new_ff QStandardItem();
  item->setData(title, kTitleRole);
  item->setData(filename, kFilenameRole);
  item->setData(settings["description"].toString(), kDescriptionRole);
  item->setData(settings["auto_run"], kAutoRunRole);
  item->setData(settings["auto_run_interval"], kAutoRunIntervalRole);
  item->setData(settings["hide_passwords"], kHidePasswordsRole);
  item->setData(settings["lock_links"], kLockLinksRole);
  item->setData(settings["max_node_posts"], kMaxNodePostsRole);
  item->setData(settings["browser_width"], kBrowserWidthRole);
  item->setData(settings["browser_height"], kBrowserHeightRole);
  setItem(_working_row, 0, item);

  // Clear the existing nodes.
  // Most of the child entities of the root group entity are nodes.
  get_root_group()->destroy_all_children();
  // Except for the "inputs", "outputs", and "links" namespace entities. So we recreated them here.
  get_dep<Compute>(get_root_group())->create_inputs_outputs();

  // Now sort everyting.
  sort_files();
  save_model();

  // Save the new empty graph as the new graph.
  save_graph();
}

void FileModel::destroy_graph(int row) {
  external();
  if (rowCount() <= 1) {
    return;
  }

  if (row == _working_row) {
    if (row == 0) {
      load_graph(1);
    } else {
      load_graph(0);
    }
  }

  // Get the current title.
  QString title = data(index(_working_row,0), kTitleRole).toString();

  // Get the filename at row.
  QString filename = data(index(row,0), kFilenameRole).toString();
  destroy_file(filename);

  // Remove the row.
  removeRows(row,1);

  // Update the current row.
  _working_row = find_index(title);

  // Now save the model.
  save_model();
}

// The info argument does not need to contain all key-value pairs.
// Only those key-value pairs present will be updated.
void FileModel::update_graph(int row, const QVariantMap& settings) {
  external();
  QVariantMap::const_iterator iter;
  for (iter = settings.begin(); iter != settings.end(); ++iter) {
    set_setting(row, iter.key(), iter.value());
  }
  // Sort the files.
  sort_files();
  save_model();
}

}

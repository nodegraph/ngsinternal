#include <gui/quick/filemodel.h>
#include <base/objectmodel/deploader.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <base/utils/bits.h>
#include <base/utils/crypto.h>

#include <components/interactions/graphbuilder.h>
#include <components/entities/entityids.h>
#include <gui/quick/nodegraphquickitemglobals.h>

#include <cstddef>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QDebug>

namespace ngs {

const QString FileModel::kCryptoFile = "app_init.dat";
const QString FileModel::kAppFile = "app_data.dat";
const QString FileModel::kAppDir = "app_data";

FileModel::FileModel()
    : QStandardItemModel(),
      Component(g_app_root, kIID(), kDID()),
      _graph_builder(this),
      _working_row(-1),
      _use_encryption(true){
  get_dep_loader()->register_fixed_dep(_graph_builder, "");

  // Make sure the data dir exists.
  QString data_dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
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
  QHash<int, QByteArray> roles;
  roles[Qt::UserRole] = "title";
  roles[kFilenameRole] = "filename";
  roles[kDescriptionRole] = "description";
  setItemRoleNames(roles);

  // Sort on titles.
  setSortRole(Qt::UserRole);
  sort(0,Qt::AscendingOrder);

  // Connect signals.
  connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_item_changed(QStandardItem*)));
}

FileModel::~FileModel() {
}

void FileModel::initialize_fixed_deps() {
  Component::initialize_fixed_deps();
}

void FileModel::on_item_changed(QStandardItem* item) {
  save_model();
}

void FileModel::create_crypto(const QString& chosen_password) {
  assert(_nonce.empty());
  assert(_key.empty());

  _nonce = Crypto::generate_nonce();
  _salt = Crypto::generate_salt();
  _hashed_password = Crypto::generate_hashed_password(chosen_password.toStdString());
  _key = Crypto::generate_private_key(chosen_password.toStdString(), _salt); // The key is never saved.

  // Save out the crypto config.
  {
    std::stringstream ss;
    SimpleSaver saver(ss);
    saver.save(_nonce);
    saver.save(_salt);
    saver.save(_hashed_password);
    write_file(kCryptoFile, ss.str());
  }
}

bool FileModel::crypto_exists() const {
  if (file_exists(kCryptoFile)) {
    return true;
  }
  return false;
}

void FileModel::load_crypto() {
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
}

bool FileModel::check_password(const QString& password) {
  if (!Crypto::check_password(password.toStdString(), _hashed_password)) {
    return false;
  }
  // Generate the private key.
  _key = Crypto::generate_private_key(password.toStdString(), _salt);
  return true;
}


void FileModel::sort_files() {
  QString title = data(index(_working_row,0), Qt::UserRole).toString();
  sort(0,Qt::AscendingOrder);
  _working_row = find_index(title);
  assert(_working_row>=0);
}

int FileModel::find_index(const QString& title) const {
  for (int i=0; i<rowCount(); ++i) {
    if (title == data(index(i,0), Qt::UserRole).toString()) {
      return i;
    }
  }
  return -1;
}

int FileModel::get_working_row() const {
  return _working_row;
}

QString FileModel::get_working_title() const {
  return get_title(_working_row);
}

QString FileModel::get_title(int row) const {
  return data(index(row,0), Qt::UserRole).toString();
}

QString FileModel::get_description(int row) const {
  return data(index(row,0), kDescriptionRole).toString();
}

QString FileModel::get_prefixed_file(const QString& file) const {
  return _app_dir + "/" + file;
}

// Data operations.
std::string FileModel::encrypt_data(const std::string& data) const {
  return Crypto::encrypt(data, _key, _nonce);
}

std::string FileModel::decrypt_data(const std::string& encrypted_data) const {
  return Crypto::decrypt(encrypted_data, _key, _nonce);
}

void FileModel::write_file(const QString& filename, const std::string& data, bool encrypt) const {
  QString full_name = get_prefixed_file(filename);
  std::cerr << "saving to file: " << full_name.toStdString() << "\n";

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
  QString full_name = get_prefixed_file(filename);
  std::cerr << "loading from file: " << full_name.toStdString() << "\n";

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
  QString full_name = get_prefixed_file(filename);
  // Erase the file.
  QFile file(full_name);
  file.remove();
}

bool FileModel::title_exists(const QString& title) const {
  for (int i=0; i<rowCount(); ++i) {
    if (title == data(index(i,0), Qt::UserRole).toString()) {
      return true;
    }
  }
  return false;
}

QString FileModel::make_title_unique(const QString& title) const {
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
  QString full = get_prefixed_file(filename);
  QFileInfo info(full);
  if (info.exists()) {
    return true;
  }
  return false;
}

QString FileModel::make_filename_unique(const QString& filename) const {
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

    QStandardItem *item = new_ff QStandardItem();
    QString qtitle(title.c_str());
    item->setData(qtitle, Qt::UserRole);
    item->setData(filename.c_str(), kFilenameRole);
    item->setData(description.c_str(), kDescriptionRole);
    setItem(i, 0, item);
    setItem(i, 0, item);
  }

  assert(_working_row >= 0);
}

void FileModel::save_model() const {
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
      saver.save(data(index(i,0), Qt::UserRole).toString().toStdString());
      saver.save(data(index(i,0), kFilenameRole).toString().toStdString());
      saver.save(data(index(i,0), kDescriptionRole).toString().toStdString());
    }
  }

  // Save the data.
  write_file(kAppFile, ss.str(), _use_encryption);
}

void FileModel::load_graph() {
  if (_working_row < 0) {
    // Create the first graph file.
    create_graph("demo", "A simple demo.");
    // Build the default graph.
    _graph_builder->build_test_graph();
    get_app_root()->initialize_deps();
    get_app_root()->update_deps_and_hierarchy();
    save_graph();
  } else {
    load_graph(_working_row);
  }
}

void FileModel::save_graph() {
  if (_working_row < 0) {
    return;
  }
  save_graph(_working_row);
}

void FileModel::load_graph(int row) {
  _working_row = row;

  // Load the graph file.
  QString graph_file = data(index(row,0), kFilenameRole).toString();
  QByteArray contents = load_file(graph_file, true);

  // Now load the data into the app root entity.
  Bits* bits = create_bits_from_raw(contents.data(),contents.size());
  SimpleLoader loader(bits);

  // Load off some pre data.
  size_t derived_id;
  loader.load(derived_id);
  std::string name;
  loader.load(name);

  //get_root_group()->destroy_all_children();
  get_root_group()->load(loader);

  // Although everything down from the root group is updated by the load.
  // Everything from the app root is not updated. So we update it here.
  get_app_root()->initialize_deps();
  get_app_root()->update_deps_and_hierarchy();

  // Save the model with the latest row.
  save_model();
}

void FileModel::save_graph(int row) {
  _working_row = row;

  // Save the graph to a string.
  std::stringstream ss;
  {
    SimpleSaver saver(ss);
    this->get_root_group()->save(saver);
  }

  // Write the graph file.
  QString graph_file = data(index(row,0), kFilenameRole).toString();
  write_file(graph_file, ss.str(), _use_encryption);
}

void FileModel::create_graph(const QString& title, const QString& description) {
  // Add a new row.
  setRowCount(rowCount()+1);
  _working_row = rowCount() -1;

  // Create a new item.
  QString filename = make_filename_unique();
  QStandardItem *item = new_ff QStandardItem();
  item->setData(title, Qt::UserRole);
  item->setData(filename, kFilenameRole);
  item->setData(description, kDescriptionRole);
  setItem(_working_row, 0, item);

  // Build the existing graph.
  std::unordered_set<size_t> dids;
  dids.insert(kBaseNamespaceEntity);
  get_root_group()->destroy_all_children_except(dids);

  // Now sort everyting.
  sort_files();
  save_model();

  // Save the new empty graph as the new graph.
  save_graph();
}

void FileModel::destroy_graph(int row) {
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
  QString title = data(index(_working_row,0), Qt::UserRole).toString();

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

void FileModel::update_graph(int row, const QString& title, const QString& description) {
  QString row_title = data(index(row,0),Qt::UserRole).toString();
  if (row_title != title) {
    row_title = make_title_unique(title);
  }

  QStandardItem* it = item(row, 0);
  it->setData(row_title, Qt::UserRole);
  it->setData(description, kDescriptionRole);
  setItem(row,0,it);

  // Sort the files.
  sort_files();
  save_model();
}

}

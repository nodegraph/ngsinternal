#include <base/objectmodel/deploader.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <base/utils/bits.h>
#include <base/utils/crypto.h>

#include <ngsversion.h>

#include <components/interactions/graphbuilder.h>

#include <base/objectmodel/appconfig.h>
#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/comms/cryptologic.h>
#include <guicomponents/quick/nodegraphquickitem.h>

#include <base/objectmodel/entityids.h>
#include <entities/guientities.h>

#include <cstddef>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <guicomponents/computes/taskqueuer.h>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtQml/QtQml>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlApplicationEngine>

namespace ngs {

const QString FileModel::kAppFile = "model.dat"; // Stores the mapping between displayed filename and actual filename.
const int FileModel::kMaxConcurrentDownloadsLite = 1;

FileModel::FileModel(Entity* app_root)
    : QStandardItemModel(),
      Component(app_root, kIID(), kDID()),
      _license_checker(this),
      _crypto_logic(this),
      _graph_builder(this),
      _working_row(-1) {
  get_dep_loader()->register_fixed_dep(_license_checker, Path());
  get_dep_loader()->register_fixed_dep(_crypto_logic, Path());
  get_dep_loader()->register_fixed_dep(_graph_builder, Path());

  // Set our role names.
  _roles[kTitleRole] = "title";
  _roles[kFilenameRole] = "filename";
  _roles[kDescriptionRole] = "description";
  _roles[kAutoRunRole] = "auto_run";
  _roles[kAutoRunIntervalRole] = "auto_run_interval";
  _roles[kLockLinksRole] = "lock_links";
  _roles[kMaxNodePostsRole] = "max_node_posts";
  _roles[kMaxConcurrentDownloadsRole] = "max_concurrent_downloads";
  _roles[kDefaultDownloadsDirRole] = "default_downloads_directory";
  setItemRoleNames(_roles);

  // Set up our default role values.
  _default_settings["title"] = "untitled";
  _default_settings["description"] = "An empty graph.";
  _default_settings["auto_run"] = false;
  _default_settings["auto_run_interval"] = 60;
  _default_settings["lock_links"] = false;
  _default_settings["max_node_posts"] = 1000;
  _default_settings["max_concurrent_downloads"] = 2;
  _default_settings["default_downloads_directory"] = "";

  // Sort on titles.
  setSortRole(kTitleRole);
  sort(0,Qt::AscendingOrder);

  // Connect signals.
  connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_item_changed(QStandardItem*)));
}

FileModel::~FileModel() {
}

//int FileModel::get_max_node_posts() const {
//  return get_work_setting(FileModel::kMaxNodePostsRole).toInt();
//}
//
//void FileModel::set_max_node_posts(int max) {
//  set_work_setting(FileModel::kMaxNodePostsRole, max);
//  emit max_node_posts_changed();
//}

bool FileModel::links_are_locked() const {
  return get_work_setting(FileModel::kLockLinksRole).toInt();
}

int FileModel::get_max_concurrent_downloads() const {
  if (_license_checker->has_valid_pro_license()) {
    // Get the number of concurrent downloads from the settings.
    return get_work_setting(FileModel::kMaxConcurrentDownloadsRole).toInt();
  }
  return kMaxConcurrentDownloadsLite;
}

QString FileModel::get_default_download_dir() const {
  if (_license_checker->has_valid_pro_license()) {
    // Get the number of concurrent downloads from the settings.
    QString dir = get_work_setting(FileModel::kDefaultDownloadsDirRole).toString();
    if (dir.isEmpty()) {
      return AppConfig::get_fallback_download_dir();
    }
    return dir;
  }
  return AppConfig::get_fallback_download_dir();
}

void FileModel::on_item_changed(QStandardItem* item) {
  external();
  save_model();
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

Q_INVOKABLE QString FileModel::get_current_title() const {
  return get_setting(_working_row, Role::kTitleRole).toString();
}

Q_INVOKABLE QString FileModel::get_current_description() const {
  return get_setting(_working_row, Role::kDescriptionRole).toString();
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

void FileModel::load_model() {
  external();
  QByteArray contents = _crypto_logic->load_file(kAppFile);
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
    bool lock_links;
    loader.load(lock_links);
    int max_node_posts;
    loader.load(max_node_posts);
    int max_concurrent_downloads;
    loader.load(max_concurrent_downloads);
    std::string default_downloads_directory;
    loader.load(default_downloads_directory);

    QStandardItem *item = new_ff QStandardItem();
    QString qtitle(title.c_str());
    item->setData(qtitle, kTitleRole);
    item->setData(filename.c_str(), kFilenameRole);
    item->setData(description.c_str(), kDescriptionRole);
    item->setData(auto_run, kAutoRunRole);
    item->setData(auto_run_interval, kAutoRunIntervalRole);
    item->setData(lock_links, kLockLinksRole);
    item->setData(max_node_posts, kMaxNodePostsRole);
    item->setData(max_concurrent_downloads, kMaxConcurrentDownloadsRole);
    item->setData(default_downloads_directory.c_str(), kDefaultDownloadsDirRole);
    setItem(static_cast<int>(i), 0, item);
  }

  //assert(_working_row >= 0);

  // Update qml.
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
      int ii = static_cast<int>(i);
      saver.save(data(index(ii,0), kTitleRole).toString().toStdString());
      saver.save(data(index(ii,0), kFilenameRole).toString().toStdString());
      saver.save(data(index(ii,0), kDescriptionRole).toString().toStdString());
      saver.save(data(index(ii,0), kAutoRunRole).toBool());
      saver.save(data(index(ii,0), kAutoRunIntervalRole).toInt());
      saver.save(data(index(ii,0), kLockLinksRole).toBool());
      saver.save(data(index(ii,0), kMaxNodePostsRole).toInt());
      saver.save(data(index(ii,0), kMaxConcurrentDownloadsRole).toInt());
      saver.save(data(index(ii,0), kDefaultDownloadsDirRole).toString().toStdString());
    }
  }

  // Save the data.
  _crypto_logic->write_file(kAppFile, ss.str());

  // Update qml.
  emit max_node_posts_changed();
}

void FileModel::load_graph() {
  external();
  if (_working_row < 0) {
    // Create the first graph file.
    QVariantMap info;
    info["title"] = "untitled";
    info["description"] = "you can rename this file";
    create_graph(info);
//    // Build the default graph.
//    _graph_builder->build_test_graph();
//    get_app_root()->initialize_wires();
//    get_app_root()->clean_wires();
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

bool FileModel::user_macro_exists(const QString& macro_name) const {
  QString full = AppConfig::get_user_macros_dir() + "/" + macro_name;
  QFileInfo info(full);
  if (info.exists()) {
    return true;
  }
  return false;
}

void FileModel::publish_user_macro(const QString& macro_name) {
  external();
  if (_working_row < 0) {
    return;
  }
  publish_user_macro(_working_row, macro_name);
}

void FileModel::load_graph(int row) {
  std::cerr << "Starting to load graph at row: " << row << "\n";
  external();
  _working_row = row;

  // Load the graph file.
  QString graph_file = data(index(row,0), kFilenameRole).toString();
  QByteArray contents = _crypto_logic->load_file(graph_file);

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

  std::string description;
  loader.load(description);

  // Load off some pre data.
  size_t derived_id;
  loader.load(derived_id);
  std::string name;
  loader.load(name);

  std::cerr << "starting load process\n";

  //get_root_group()->destroy_all_children();
  get_root_group()->load(loader);

  std::cerr << "finished loading components\n";

  // Although everything down from the root group is updated by the load.
  // Everything from the app root is not updated. So we update it here.
  get_root_group()->initialize_wires();

  std::cerr << "finished initializing wires\n";

  get_app_root()->clean_wires();

  std::cerr << "finished cleaning wires\n";

  // Save the model with the latest row.
  save_model();
}

std::string FileModel::graph_to_string(int row) {
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

    // Save the description.
    std::string description = data(index(row,0), kDescriptionRole).toString().toStdString();
    saver.save(description);

    // Save the group contents.
    this->get_root_group()->save(saver);
  }
  return ss.str();
}

void FileModel::save_graph(int row) {
  external();
  _working_row = row;

  // Save the graph to a string.
  std::string contents = graph_to_string(row);

  // Write the graph file.
  QString graph_file = data(index(row,0), kFilenameRole).toString();
  _crypto_logic->write_file(graph_file, contents);
}

void FileModel::publish_user_macro(int row, const QString& macro_name) {
  external();
  _working_row = row;

  // Save the graph to a string.
  std::string contents = graph_to_string(row);

  // The name of the macro file will be the same as the title of graph used to create it.
  QString full_name = AppConfig::get_user_macros_dir() + "/" + macro_name;

  // Write out the file without any encryption.
  QFile file(full_name);
  file.open(QIODevice::WriteOnly);
  file.write(contents.c_str(), contents.size());
  file.close();
}

QStringList get_files(const QString& dir_path) {
  QDir dir(dir_path);
  QFileInfoList list = dir.entryInfoList(QDir::Files,QDir::Name);
  QStringList filenames;
  for (int i=0; i<list.size(); ++i) {
    QString filename = list[i].fileName();
    filenames.push_back(filename);
  }
  return filenames;
}

QStringList FileModel::get_user_macro_names() const {
  return get_files(AppConfig::get_user_macros_dir());
}

QStringList FileModel::get_app_macro_names() const {
  return get_files(AppConfig::get_app_macros_dir());
}

void FileModel::destroy_graph() {
  external();
  destroy_graph(_working_row);
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
  QString filename = _crypto_logic->make_filename_unique();

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
  item->setData(settings["lock_links"], kLockLinksRole);
  item->setData(settings["max_node_posts"], kMaxNodePostsRole);
  item->setData(settings["max_concurrent_downloads"], kMaxConcurrentDownloadsRole);
  item->setData(settings["default_downloads_directory"], kDefaultDownloadsDirRole);
  setItem(_working_row, 0, item);

  // Clear the existing nodes.
  // Most of the child entities of the root group entity are nodes.
  get_root_group()->destroy_all_children();
  // Except for the "inputs", "outputs", and "links" namespace entities. So we recreated them here.
  get_dep<Compute>(get_root_group())->create_inputs_outputs(EntityConfig());

  // Although everything down from the root group is updated.
  // Everything from the app root is not updated. So we update it here.
  get_root_group()->initialize_wires();
  get_app_root()->clean_wires();

  // Now sort everyting.
  sort_files();
  save_model();
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
  _crypto_logic->destroy_file(filename);

  // Remove the row.
  removeRows(row,1);

  // Update the current row.
  _working_row = find_index(title);

  // Now save the model.
  save_model();
}

void FileModel::update_current_graph(const QVariantMap& info) {
  update_graph(_working_row, info);
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

#include <gui/quick/filemodel.h>
#include <base/objectmodel/deploader.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <base/utils/bits.h>

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

const QString FileModel::kAppFile = "app_data.dat";
const QString FileModel::kAppDir = "app_data";

FileModel::FileModel()
    : QStandardItemModel(),
      Component(g_app_root, kIID(), kDID()),
      _graph_builder(this),
      _working_row(-1){
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

  // Load our model.
  load_model();

  // Connect signals.
  connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(on_item_changed(QStandardItem*)));
}

FileModel::~FileModel() {
}

void FileModel::initialize_fixed_deps() {
  Component::initialize_fixed_deps();
}

void FileModel::on_item_changed(QStandardItem* item) {
  save_graph(_working_row);
  save_model();
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

QString FileModel::get_app_filename() {
  return get_prefixed_file(kAppFile);
}

QString FileModel::get_prefixed_file(const QString& file) {
  return _app_dir + "/" + file;
}

bool FileModel::title_exists(const QString& title) {
  for (int i=0; i<rowCount(); ++i) {
    if (title == data(index(i,0), Qt::UserRole).toString()) {
      return true;
    }
  }
  return false;
}

QString FileModel::make_title_unique(const QString& title) {
  std::string name = title.toStdString();
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
    if (!title_exists(name.c_str())) {
      return name.c_str();
    }
    number+=1;
  }
  return "error";
}

bool FileModel::file_exists(const QString& filename) {
  QString full = get_prefixed_file(filename);
  QFileInfo info(full);
  if (info.exists()) {
    return true;
  }
  return false;
}

QString FileModel::make_filename_unique(const QString& filename) {
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
  QString app_file = get_app_filename();

  // If this file doesn't exist there is nothing to load.
  QFileInfo info(app_file);
  if (!info.exists()) {
    return;
  }

  // Read all the bytes from the file.
  QFile file(app_file);
  file.open(QIODevice::ReadOnly);
  QByteArray contents = file.readAll();

  // Now load them.
  Bits* bits = create_bits_from_raw(contents.data(),contents.size());
  SimpleLoader loader(bits);

  // Load the current row at the time of serialization.
  loader.load(_working_row);

  // Load the number of files.
  size_t num_rows;
  loader.load(num_rows);

  setRowCount(num_rows);
  for (size_t i=0; i<num_rows; ++i) {

    std::string title;
    loader.load(title);
    std::string filename;
    loader.load(filename);
    std::string description;
    loader.load(description);

    QStandardItem *item = new_ff QStandardItem();
    item->setData(title.c_str(), Qt::UserRole);
    item->setData(filename.c_str(), kFilenameRole);
    item->setData(description.c_str(), kDescriptionRole);
    setItem(i, 0, item);
  }

  assert(_working_row >= 0);
}

void FileModel::save_model() {
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

  // Open the app file.
  QString filename = get_app_filename();
  QFile file(filename);
  file.open(QIODevice::ReadWrite);

  // Save the string to the file.
  file.write(ss.str().c_str(), ss.str().size());
  file.close();
}

void FileModel::load_graph() {
  if (_working_row < 0) {
    // Create the first graph file.
    create_graph("demo", "A simple demo.");
    // Build the default graph.
    _graph_builder->build_test_graph();
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

  // If the current filename doesn't exist, something has gone wrong.
  QString graph_file = data(index(row,0), kFilenameRole).toString();
  graph_file = get_prefixed_file(graph_file);
  std::cerr << "trying to load graph from: " << graph_file.toStdString() << "\n";

  QFileInfo info(graph_file);
  assert(info.exists());

  // Read all the bytes from the file.
  QFile file(graph_file);
  file.open(QIODevice::ReadOnly);
  QByteArray contents = file.readAll();

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

  // Although everything down from the root group is update by the load.
  // Everything from the app root is not updated. So we update it here.
  get_app_root()->initialize_deps();
  get_app_root()->update_deps_and_hierarchy();
  std::cerr << "done load graph from: " << graph_file.toStdString() << "\n";

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

  // Open the graph file.
  QString graph_file = data(index(row,0), kFilenameRole).toString();
  graph_file = get_prefixed_file(graph_file);
  std::cerr << "trying to save graph to: " << graph_file.toStdString() << "\n";

  QFile file(graph_file);
  file.open(QIODevice::ReadWrite);

  // Save the string to the file.
  file.write(ss.str().c_str(), ss.str().size());
  file.close();
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
  filename = get_prefixed_file(filename);

  // Erase the file.
  QFile file(filename);
  file.remove();

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

  if (row_title != title) {
    sort_files();
  }
}

}

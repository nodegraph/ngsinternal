#pragma once
#include <gui/gui_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <components/entities/componentids.h>

#include <QtGui/QStandardItemModel>

namespace ngs {

class GraphBuilder;

class GUI_EXPORT FileModel: public QStandardItemModel, public Component {
  Q_OBJECT
 public:

  static const QString kCryptoFile;
  static const QString kAppFile;
  static const QString kAppDir;

  COMPONENT_ID(FileModel, FileModel)

  enum Role {
    kFilenameRole,
    kDescriptionRole,
  };

  FileModel();
  virtual ~FileModel();

  virtual void initialize_fixed_deps();

  // Password Setup.
  Q_INVOKABLE void create_crypto(const std::string& chosen_password);
  Q_INVOKABLE bool crypto_exists() const;
  Q_INVOKABLE void load_crypto();
  Q_INVOKABLE bool check_password(const std::string& password);

  // Graph.
  Q_INVOKABLE void load_graph();
  Q_INVOKABLE void save_graph();
  Q_INVOKABLE void load_graph(int row);
  Q_INVOKABLE void save_graph(int row);
  Q_INVOKABLE void create_graph(const QString& title, const QString& description);
  Q_INVOKABLE void destroy_graph(int row);
  Q_INVOKABLE void update_graph(int row, const QString& title, const QString& description);

  // The working row has the file that the app is currently working with.
  // It is different from the list view's current index, which highlights
  // the currently selected item in the gui. This is not always the same
  // as the working row.
  Q_INVOKABLE int get_working_row() const;
  Q_INVOKABLE QString get_working_title() const;

  // Title and Description.
  Q_INVOKABLE QString make_title_unique(const QString& title) const;
  Q_INVOKABLE QString get_title(int row) const;
  Q_INVOKABLE QString get_description(int row) const;

 public Q_SLOTS:
  void on_item_changed(QStandardItem* item);


 private:
  QString get_prefixed_file(const QString& file) const;

  // File operations.
  void write_file(const QString& filename, const std::string& data) const;
  QByteArray load_file(const QString& filename) const;
  void destroy_file(const QString& filename) const;
  bool file_exists(const QString& filename) const;
  QString make_filename_unique(const QString& filename=QString("graph1")) const;

  // Title.
  bool title_exists(const QString& title) const;

  // Model.
  void load_model();
  void save_model() const;
  void sort_files();
  int find_index(const QString& title) const;

  // Our Dependencies.
  Dep<GraphBuilder> _graph_builder;

  // Caches.
  QString _app_dir;
  int _working_row;

  // Crypto.
  std::string _nonce;
  std::string _salt;
  std::string _hashed_password;
  std::string _key;

};

}

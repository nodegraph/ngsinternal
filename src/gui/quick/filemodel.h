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
    kTitleRole = Qt::UserRole, // String: The fake filename displayed in the gui.
    kFilenameRole, // String: The real filename.
    kDescriptionRole, // String: A short description of the contents of the file.
    kAutoRunRole, // Bool: Whether to auto run this file.
    kAutoRunIntervalRole, // Int: Interval between runs of this file in minutes.
    kHidePasswordsRole, // Bool: Whether to hide passwords.
    kLockLinksRole, // Bool: Whether to lock links.
    kMaxNodePostsRole, // Int: Maximum number of node posts. Overflow is destroyed.
  };

  FileModel();
  virtual ~FileModel();

  virtual void initialize_fixed_deps();

  // Password Setup.
  Q_INVOKABLE void create_crypto(const QString& chosen_password);
  Q_INVOKABLE bool crypto_exists() const;
  Q_INVOKABLE void load_crypto();
  Q_INVOKABLE bool check_password(const QString& password);

  // Model.
  Q_INVOKABLE void load_model();

  // Graph.
  Q_INVOKABLE void load_graph();
  Q_INVOKABLE void save_graph();
  Q_INVOKABLE void load_graph(int row);
  Q_INVOKABLE void save_graph(int row);

  Q_INVOKABLE QVariantMap get_default_info();
  Q_INVOKABLE void create_graph(const QVariantMap& info);
  Q_INVOKABLE void destroy_graph(int row);
  Q_INVOKABLE void update_graph(int row, const QVariantMap& info);

  // The working row has the file that the app is currently working with.
  // It is different from the list view's current index, which highlights
  // the currently selected item in the gui. This is not always the same
  // as the working row.
  Q_INVOKABLE int get_working_row() const;
  Q_INVOKABLE QString get_working_title() const;

  // Title generators.
  Q_INVOKABLE QString make_title_unique(const QString& title) const;

  // Getters.
  Q_INVOKABLE QString get_title(int row) const;
  Q_INVOKABLE QString get_description(int row) const;
  Q_INVOKABLE bool get_auto_run(int row) const;
  Q_INVOKABLE int get_auto_run_interval(int row) const;
  Q_INVOKABLE bool get_hide_passwords(int row) const;
  Q_INVOKABLE bool get_lock_links(int row) const;
  Q_INVOKABLE int get_max_node_posts(int row) const;

  // Setters.
  Q_INVOKABLE void set_title(int row, const QString& title);
  Q_INVOKABLE void set_description(int row, const QString& description);
  Q_INVOKABLE void set_auto_run(int row, bool auto_run);
  Q_INVOKABLE void set_auto_run_interval(int row, int auto_run_interval);
  Q_INVOKABLE void set_hide_passwords(int row, bool hide_passwords);
  Q_INVOKABLE void set_lock_links(int row, bool lock_links);
  Q_INVOKABLE void set_max_node_posts(int row, int max_node_posts);

  // Sort rows.
  Q_INVOKABLE void sort_files();

 public Q_SLOTS:
  void on_item_changed(QStandardItem* item);


 private:
  QString get_prefixed_file(const QString& file) const;

  // Cipher operations.
  std::string encrypt_data(const std::string& data) const;
  std::string decrypt_data(const std::string& cipher_text) const;

  // File operations.
  void write_file(const QString& filename, const std::string& data, bool encrypt = false) const;
  QByteArray load_file(const QString& filename, bool decrypt = false) const;
  void destroy_file(const QString& filename) const;
  bool file_exists(const QString& filename) const;
  QString make_filename_unique(const QString& filename=QString("graph1")) const;

  // Title.
  bool title_exists(const QString& title) const;

  // Model.
  void save_model() const;
  int find_index(const QString& title) const;

  // Our Dependencies.
  Dep<GraphBuilder> _graph_builder;

  // Caches.
  QString _app_dir;
  int _working_row;

  // Crypto.
  bool _use_encryption;
  std::string _nonce;
  std::string _salt;
  std::string _hashed_password;
  std::string _key;

};

}

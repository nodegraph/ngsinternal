#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/componentids.h>

#include <QtGui/QStandardItemModel>

namespace ngs {

class LicenseChecker;
class CryptoLogic;
class GraphBuilder;

class COMMS_EXPORT FileModel: public QStandardItemModel, public Component {
  Q_OBJECT
 public:

  static const QString kAppFile;
  static const int kMaxConcurrentDownloadsLite;
  static const int kMaxSaveableNodes;

  COMPONENT_ID(FileModel, FileModel)

  enum Role {
    kTitleRole = Qt::UserRole, // String: The fake filename displayed in the gui.
    kFilenameRole, // String: The real filename.
    kDescriptionRole, // String: A short description of the contents of the file.
    kAutoRunRole, // Bool: Whether to auto run this file.
    kAutoRunIntervalRole, // Int: Interval between runs of this file in minutes.
    kLockLinksRole, // Bool: Whether to lock links.
    kMaxNodePostsRole, // Int: Maximum number of node posts. Overflow is destroyed.
    kMaxConcurrentDownloadsRole, // Int: Maximum number of concurrent downloads.
    kDefaultDownloadsDirRole, // String: The default download directory.
  };

  FileModel(Entity* app_root);
  virtual ~FileModel();

  // Properties that can be bound in QML.
  Q_PROPERTY(int max_node_posts READ get_max_node_posts NOTIFY max_node_posts_changed)
  Q_PROPERTY(int auto_run READ get_auto_run NOTIFY auto_run_changed)
  Q_PROPERTY(int auto_run_interval READ get_auto_run_interval NOTIFY auto_run_interval_changed)

  Q_INVOKABLE int get_max_node_posts() const;
  Q_INVOKABLE bool get_auto_run() const;
  Q_INVOKABLE int get_auto_run_interval() const;

  bool links_are_locked() const;
  int get_max_concurrent_downloads() const;
  QString get_default_download_dir() const;

  // Model.
  Q_INVOKABLE void load_model();

  // Graph.
  Q_INVOKABLE void load_graph();
  Q_INVOKABLE void load_graph(int row);

  Q_INVOKABLE bool save_graph() const;
  Q_INVOKABLE bool save_graph(int row) const;

  Q_INVOKABLE bool destroy_graph();
  Q_INVOKABLE bool destroy_graph(int row);

  Q_INVOKABLE bool public_macro_exists(const QString& macro_name) const;
  Q_INVOKABLE bool private_macro_exists(const QString& macro_name) const;

  Q_INVOKABLE void publish_public_macro(const QString& macro_name);
  Q_INVOKABLE void publish_public_macro(int row, const QString& macro_name);

  Q_INVOKABLE void publish_private_macro(const QString& macro_name);
  Q_INVOKABLE void publish_private_macro(int row, const QString& macro_name);

  Q_INVOKABLE QStringList get_public_macro_names() const;
  Q_INVOKABLE QStringList get_private_macro_names() const;
  Q_INVOKABLE QStringList get_app_macro_names() const;

  std::string graph_to_string(int row) const;

  Q_INVOKABLE QVariantMap get_default_settings() {return _default_settings;} // This has to be returned by value, otherwise QML gets 'undefined'.
  Q_INVOKABLE void create_graph(const QVariantMap& info);

  Q_INVOKABLE void update_graph(int row, const QVariantMap& info);

  // The working row has the file that the app is currently working with.
  // It is different from the list view's current index, which highlights
  // the currently selected item in the gui. This is not always the same
  // as the working row.
  Q_INVOKABLE int get_working_row() const;
  Q_INVOKABLE QString get_current_title() const;
  Q_INVOKABLE QString get_current_description() const;
  Q_INVOKABLE void update_current_graph(const QVariantMap& info);

  // Title generators.
  Q_INVOKABLE QString make_title_unique(const QString& title) const;

  // Getters.
  Q_INVOKABLE int get_role(const QString& role_name) const;
  Q_INVOKABLE QVariant get_setting(int row, int role) const;
  Q_INVOKABLE QVariant get_setting(int row, const QString& role_name) const;

  // Setters.
  Q_INVOKABLE void set_setting(int row, int role, const QVariant& value);
  Q_INVOKABLE void set_setting(int row, const QString& role_name, const QVariant& value);

  // Working Row Getters.
  Q_INVOKABLE QVariant get_work_setting(int role) const;
  Q_INVOKABLE QVariant get_work_setting(const QString& role_name) const;

  // Working Row Setters.
  Q_INVOKABLE void set_work_setting(int role, const QVariant& value);
  Q_INVOKABLE void set_work_setting(const QString& role_name, const QVariant& value);

  // Sort rows.
  Q_INVOKABLE void sort_files();

 public Q_SLOTS:
  void on_item_changed(QStandardItem* item);

 Q_SIGNALS:
   void max_node_posts_changed(int);
   void auto_run_changed(bool);
   void auto_run_interval_changed(int);

 private:
  // Title.
  bool title_exists(const QString& title) const;

  // Model.
  void save_model(); // This could be const but as a hack it's non const to make this object dirty on calling it.
  int find_index(const QString& title) const;

  // Update qml.
  void update_qml() const;

  // Our Dependencies.
  Dep<LicenseChecker> _license_checker;
  Dep<CryptoLogic> _crypto_logic;
  Dep<GraphBuilder> _graph_builder;

  // Caches.
  int _working_row;

  // Our roles.
  QHash<int, QByteArray> _roles;

  // Default File Settings.
  QVariantMap _default_settings;

};

}

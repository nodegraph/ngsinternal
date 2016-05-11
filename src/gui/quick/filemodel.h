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

  Q_INVOKABLE void load_graph(int row);
  Q_INVOKABLE void save_graph(int row);
  Q_INVOKABLE void create_graph(const QString& title, const QString& description);
  Q_INVOKABLE void destroy_graph(int row);
  Q_INVOKABLE void update_graph(int row, const QString& title, const QString& description);

  Q_INVOKABLE void load_graph();
  Q_INVOKABLE void save_graph();

  Q_INVOKABLE QString make_title_unique(const QString& title);

  Q_INVOKABLE int get_working_row() const;
  Q_INVOKABLE QString get_working_title() const;
  Q_INVOKABLE QString get_title(int row) const;
  Q_INVOKABLE QString get_description(int row) const;



 public Q_SLOTS:
  void on_item_changed(QStandardItem* item);


 private:
  QString get_app_filename();
  QString get_prefixed_file(const QString& file);
  bool title_exists(const QString& title);
  bool file_exists(const QString& filename);

  QString make_filename_unique(const QString& filename=QString("graph1"));

  void load_model();
  void save_model();

  void sort_files();
  int find_index(const QString& title) const;

  Dep<GraphBuilder> _graph_builder;

  QString _app_dir;
  int _working_row;
};

}

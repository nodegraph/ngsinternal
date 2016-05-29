#pragma once
#include <QtCore/QObject>
#include <iostream>
#include <vector>

namespace ngs {

class Entity;
class Node;
class NodeGraphSelection;
class NodeGraphQuickItem;
class VariantMapTreeModel;





class CppBridge : public QObject {
Q_OBJECT
 public:
  explicit CppBridge(QObject *parent = 0);

//  Q_INVOKABLE void destroy_selection();

signals:
//void message(const QString &message);

 public slots:

 // App Loading.
 void set_max_load_progress(size_t max);
 void mark_load_progress();
 size_t get_load_progress();
 void set_marker_1(size_t mark);
 bool at_marker_1();
 bool is_finished_loading();

 // Event Loop.
 void process_events();

 // Testing.
  void on_test_message(const QString &message);
  void on_test_1(const QVariant& value);
  void on_move_root(int child_row);

  void post_init();

  void reset_input_method();

  void press_down_key(QObject* obj);
  void release_down_key(QObject* obj);
};

}

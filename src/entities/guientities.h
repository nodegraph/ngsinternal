#pragma once
#include <base/objectmodel/entity.h>
#include <entities/entities_export.h>
#include <entities/entityids.h>
#include <components/computes/inputcompute.h>

class QSurfaceFormat;
class QJsonValue;

namespace ngs {

enum class JSType;

class FileModel;
class AppComm;
class NodeJSWorker;
class BrowserRecorder;
class LicenseChecker;
class NodeGraphQuickItem;
class NodeGraphController;
class GraphBuilder;
class NodeGraphView;

class ENTITIES_EXPORT QMLAppEntity : public Entity {
 public:
  ENTITY_ID(QMLAppEntity, "qml app")
  QMLAppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());

  void init_view(QSurfaceFormat& format);
  void expose_to_qml();
  void embed_node_graph();

  FileModel* get_file_model();
  NodeJSWorker* get_app_worker();
  BrowserRecorder* get_app_recorder();
  LicenseChecker* get_license_checker();
  NodeGraphQuickItem* get_node_graph_quick_item();
  NodeGraphController* get_node_graph_controller();
  GraphBuilder* get_graph_builder();
  NodeGraphView* get_node_graph_view();
};

class ENTITIES_EXPORT QtAppEntity : public Entity {
 public:
  ENTITY_ID(QtAppEntity, "qt app")
  QtAppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT AppEntity : public Entity {
 public:
  ENTITY_ID(AppEntity, "app")
  AppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT LinkEntity : public Entity {
 public:
  ENTITY_ID(LinkEntity, "link head")
  LinkEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT GroupNodeEntity : public Entity {
 public:
  ENTITY_ID(GroupNodeEntity, "group")
  GroupNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
  virtual void copy(SimpleSaver& saver, const std::unordered_set<Entity*>& children) const;
};

class ENTITIES_EXPORT ScriptGroupNodeEntity : public GroupNodeEntity {
 public:
  ENTITY_ID(ScriptGroupNodeEntity, "script group")
 ScriptGroupNodeEntity(Entity* parent, const std::string& name):GroupNodeEntity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT BrowserGroupNodeEntity : public GroupNodeEntity {
 public:
  ENTITY_ID(BrowserGroupNodeEntity, "browser group")
  BrowserGroupNodeEntity(Entity* parent, const std::string& name):GroupNodeEntity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT FirebaseGroupNodeEntity : public GroupNodeEntity {
 public:
  ENTITY_ID(FirebaseGroupNodeEntity, "firebase group")
 FirebaseGroupNodeEntity(Entity* parent, const std::string& name):GroupNodeEntity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT MQTTGroupNodeEntity : public GroupNodeEntity {
 public:
  ENTITY_ID(MQTTGroupNodeEntity, "firebase group")
 MQTTGroupNodeEntity(Entity* parent, const std::string& name):GroupNodeEntity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT DotNodeEntity : public Entity {
 public:
  ENTITY_ID(DotNodeEntity, "dot")
  DotNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT DataNodeEntity : public Entity {
 public:
  ENTITY_ID(DataNodeEntity, "input")
 DataNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT InputNodeEntity : public Entity {
 public:
  ENTITY_ID(InputNodeEntity, "input")
  InputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT OutputNodeEntity : public Entity {
 public:
  ENTITY_ID(OutputNodeEntity, "output")
  OutputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT InputEntity : public Entity {
 public:
  ENTITY_ID(InputEntity, "input")
  InputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
  virtual void set_unconnected_value(const QJsonValue& value);
  virtual void set_exposed(bool expose);
};

class ENTITIES_EXPORT InputLabelEntity : public Entity {
 public:
  ENTITY_ID(InputLabelEntity, "input param")
  InputLabelEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT OutputEntity : public Entity {
 public:
  ENTITY_ID(OutputEntity, "output param")
  OutputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
  virtual void set_exposed(bool expose);
};

class ENTITIES_EXPORT OutputLabelEntity : public Entity {
 public:
  ENTITY_ID(OutputLabelEntity, "output param")
  OutputLabelEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT ComputeNodeEntity : public Entity {
 public:
  ENTITY_ID(ComputeNodeEntity, "compute")
 ComputeNodeEntity(Entity* parent, const std::string& name):Entity(parent, name), _did(ComponentDID::kInvalidComponent), _visible(true){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
  void set_compute_did(ComponentDID did);
  void set_visible(bool visible) {_visible = visible;}
  Compute* get_compute();
 private:
  ComponentDID _did;
  bool _visible;
};

}

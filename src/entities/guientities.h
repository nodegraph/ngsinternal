#pragma once
#include <base/objectmodel/entity.h>
#include <entities/entities_export.h>
#include <base/objectmodel/entityids.h>
#include <components/computes/inputcompute.h>

#include <unordered_map>
#include <functional>

class QSurfaceFormat;
class QJsonValue;

namespace ngs {

enum class JSType;

class CryptoLogic;
class LicenseChecker;
class FileModel;
class AppComm;
class TaskQueuer;
class BrowserRecorder;
class DownloadManager;
class NodeGraphQuickItem;
class NodeGraphController;
class GraphBuilder;
class NodeGraphView;
class BaseNodeGraphManipulator;




class ENTITIES_EXPORT QMLAppEntity : public Entity {
 public:
  ENTITY_ID(QMLAppEntity)
  QMLAppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());

  void init_view(QSurfaceFormat& format);
  void expose_to_qml();
  void embed_node_graph();

  CryptoLogic* get_crypto_logic();
  LicenseChecker* get_license_checker();
  FileModel* get_file_model();
  TaskQueuer* get_task_queuer();
  BrowserRecorder* get_app_recorder();
  DownloadManager* get_download_manager();
  NodeGraphQuickItem* get_node_graph_quick_item();
  NodeGraphController* get_node_graph_controller();
  GraphBuilder* get_graph_builder();
  NodeGraphView* get_node_graph_view();
  BaseNodeGraphManipulator* get_manipulator();
};

class ENTITIES_EXPORT QtAppEntity : public Entity {
 public:
  ENTITY_ID(QtAppEntity)
  QtAppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT AppEntity : public Entity {
 public:
  ENTITY_ID(AppEntity)
  AppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT LinkEntity : public Entity {
 public:
  ENTITY_ID(LinkEntity)
  LinkEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT GroupNodeEntity : public Entity {
 public:
  ENTITY_ID(GroupNodeEntity)
  GroupNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
  virtual void copy(SimpleSaver& saver, const std::unordered_set<Entity*>& children) const;
};

class ENTITIES_EXPORT IfGroupNodeEntity : public GroupNodeEntity {
 public:
  ENTITY_ID(IfGroupNodeEntity)
  IfGroupNodeEntity(Entity* parent, const std::string& name):GroupNodeEntity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT WhileGroupNodeEntity : public GroupNodeEntity {
 public:
  ENTITY_ID(WhileGroupNodeEntity)
 WhileGroupNodeEntity(Entity* parent, const std::string& name):GroupNodeEntity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT BrowserGroupNodeEntity : public GroupNodeEntity {
 public:
  ENTITY_ID(BrowserGroupNodeEntity)
  BrowserGroupNodeEntity(Entity* parent, const std::string& name):GroupNodeEntity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT MQTTGroupNodeEntity : public GroupNodeEntity {
 public:
  ENTITY_ID(MQTTGroupNodeEntity)
 MQTTGroupNodeEntity(Entity* parent, const std::string& name):GroupNodeEntity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT WaitNodeEntity : public Entity {
 public:
  ENTITY_ID(WaitNodeEntity)
 WaitNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT DotNodeEntity : public Entity {
 public:
  ENTITY_ID(DotNodeEntity)
  DotNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT DataNodeEntity : public Entity {
 public:
  ENTITY_ID(DataNodeEntity)
 DataNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT InputNodeEntity : public Entity {
 public:
  ENTITY_ID(InputNodeEntity)
  InputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT OutputNodeEntity : public Entity {
 public:
  ENTITY_ID(OutputNodeEntity)
  OutputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT InputEntity : public Entity {
 public:
  ENTITY_ID(InputEntity)
  InputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT InputLabelEntity : public Entity {
 public:
  ENTITY_ID(InputLabelEntity)
  InputLabelEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT OutputEntity : public Entity {
 public:
  ENTITY_ID(OutputEntity)
  OutputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT OutputLabelEntity : public Entity {
 public:
  ENTITY_ID(OutputLabelEntity)
  OutputLabelEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
};

class ENTITIES_EXPORT ComputeNodeEntity : public Entity {
 public:
  ENTITY_ID(ComputeNodeEntity)
 ComputeNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());
  Compute* get_compute();
};

class ENTITIES_EXPORT UserMacroNodeEntity : public GroupNodeEntity {
 public:
  ENTITY_ID(UserMacroNodeEntity);
  UserMacroNodeEntity(Entity* parent, const std::string& name):GroupNodeEntity(parent, name){}
  virtual void create_internals(const EntityConfig& configs = EntityConfig());

  virtual void save(SimpleSaver& saver) const;
  virtual void load_helper(SimpleLoader& loader);
  virtual void load_internals(const std::string& macro_name);
 private:
  virtual std::string get_macro_dir() const;
  std::string _macro_name;
};

class ENTITIES_EXPORT AppMacroNodeEntity : public UserMacroNodeEntity {
 public:
  ENTITY_ID(AppMacroNodeEntity);
  AppMacroNodeEntity(Entity* parent, const std::string& name):UserMacroNodeEntity(parent, name){}
 private:
  virtual std::string get_macro_dir() const;
};

}

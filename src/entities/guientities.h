#pragma once
#include <base/objectmodel/entity.h>
#include <entities/entities_export.h>
#include <entities/entityids.h>
#include <components/computes/inputcompute.h>

class QSurfaceFormat;

namespace ngs {

enum ParamType;

class FileModel;
class AppComm;
class AppWorker;
class LicenseChecker;
class NodeGraphQuickItem;
class GraphBuilder;
class NodeGraphView;

class ENTITIES_EXPORT QMLAppEntity : public Entity {
 public:
  ENTITY_ID(QMLAppEntity, "qml app")
  QMLAppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();

  void init_view(QSurfaceFormat& format);
  void expose_to_qml();
  void embed_node_graph();

  FileModel* get_file_model();
  AppComm* get_app_comm();
  AppWorker* get_app_worker();
  LicenseChecker* get_license_checker();
  NodeGraphQuickItem* get_node_graph_quick_item();
  GraphBuilder* get_graph_builder();
  NodeGraphView* get_node_graph_view();
};

class ENTITIES_EXPORT QtAppEntity : public Entity {
 public:
  ENTITY_ID(QtAppEntity, "qt app")
  QtAppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT AppEntity : public Entity {
 public:
  ENTITY_ID(AppEntity, "app")
  AppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT LinkEntity : public Entity {
 public:
  ENTITY_ID(LinkEntity, "link head")
  LinkEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT GroupNodeEntity : public Entity {
 public:
  ENTITY_ID(GroupNodeEntity, "group")
  GroupNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
  virtual void copy(SimpleSaver& saver, const std::unordered_set<Entity*>& children) const;
};

class ENTITIES_EXPORT DotNodeEntity : public Entity {
 public:
  ENTITY_ID(DotNodeEntity, "dot")
  DotNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT InputNodeEntity : public Entity {
 public:
  ENTITY_ID(InputNodeEntity, "input")
  InputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT OutputNodeEntity : public Entity {
 public:
  ENTITY_ID(OutputNodeEntity, "output")
  OutputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT MockNodeEntity : public Entity {
 public:
  ENTITY_ID(MockNodeEntity, "mock")
  MockNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT OpenBrowserNodeEntity : public Entity {
 public:
  ENTITY_ID(OpenBrowserNodeEntity, "open browser")
  OpenBrowserNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT CloseBrowserNodeEntity : public Entity {
 public:
  ENTITY_ID(CloseBrowserNodeEntity, "close browser")
  CloseBrowserNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT CreateSetFromValuesNodeEntity : public Entity {
 public:
  ENTITY_ID(CreateSetFromValuesNodeEntity, "open browser")
  CreateSetFromValuesNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT CreateSetFromTypeNodeEntity : public Entity {
 public:
  ENTITY_ID(CreateSetFromTypeNodeEntity, "open browser")
  CreateSetFromTypeNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT MouseActionNodeEntity : public Entity {
 public:
  ENTITY_ID(MouseActionNodeEntity, "mouse action")
  MouseActionNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT InputEntity : public Entity {
 public:
  ENTITY_ID(InputEntity, "input")
  InputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
  virtual void set_param_type(ParamType param_type);
  virtual void set_exposed(bool expose);
};

class ENTITIES_EXPORT InputLabelEntity : public Entity {
 public:
  ENTITY_ID(InputLabelEntity, "input param")
  InputLabelEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT OutputEntity : public Entity {
 public:
  ENTITY_ID(OutputEntity, "output param")
  OutputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
  virtual void set_param_type(ParamType param_type);
  virtual void set_exposed(bool expose);
};

class ENTITIES_EXPORT OutputLabelEntity : public Entity {
 public:
  ENTITY_ID(OutputLabelEntity, "output param")
  OutputLabelEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT ScriptNodeEntity : public Entity {
 public:
  ENTITY_ID(ScriptNodeEntity, "compute")
  ScriptNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

}

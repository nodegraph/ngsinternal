#pragma once
#include <base/objectmodel/entity.h>
#include <components/entities/entities_export.h>
#include <components/entities/entityids.h>

namespace ngs {

class ENTITIES_EXPORT QMLAppEntity : public Entity {
 public:
  ENTITY_ID(QMLAppEntity, "qml app")
  using Entity::Entity; // not supported on msvc 2013
  QMLAppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT QtAppEntity : public Entity {
 public:
  ENTITY_ID(QtAppEntity, "qt app")
  using Entity::Entity; // not supported on msvc 2013
  QtAppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT AppEntity : public Entity {
 public:
  ENTITY_ID(AppEntity, "app")
  using Entity::Entity; // not supported on msvc 2013
  AppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT LinkEntity : public Entity {
 public:
  ENTITY_ID(LinkEntity, "link head")
  using Entity::Entity;
  LinkEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT GroupNodeEntity : public Entity {
 public:
  ENTITY_ID(GroupNodeEntity, "group")
  using Entity::Entity;
  GroupNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT DotNodeEntity : public Entity {
 public:
  ENTITY_ID(DotNodeEntity, "dot")
  using Entity::Entity;
  DotNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT InputNodeEntity : public Entity {
 public:
  ENTITY_ID(InputNodeEntity, "input")
  using Entity::Entity;
  InputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT OutputNodeEntity : public Entity {
 public:
  ENTITY_ID(OutputNodeEntity, "output")
  using Entity::Entity;
  OutputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT MockNodeEntity : public Entity {
 public:
  ENTITY_ID(MockNodeEntity, "mock")
  using Entity::Entity;
  MockNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT OpenBrowserNodeEntity : public Entity {
 public:
  ENTITY_ID(OpenBrowserNodeEntity, "open browser")
  using Entity::Entity;
  OpenBrowserNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT CloseBrowserNodeEntity : public Entity {
 public:
  ENTITY_ID(CloseBrowserNodeEntity, "close browser")
  using Entity::Entity;
  CloseBrowserNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT CreateSetFromValuesNodeEntity : public Entity {
 public:
  ENTITY_ID(CreateSetFromValuesNodeEntity, "open browser")
  using Entity::Entity;
  CreateSetFromValuesNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT CreateSetFromTypeNodeEntity : public Entity {
 public:
  ENTITY_ID(CreateSetFromTypeNodeEntity, "open browser")
  using Entity::Entity;
  CreateSetFromTypeNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT InputEntity : public Entity {
 public:
  ENTITY_ID(InputEntity, "input")
  using Entity::Entity;
  InputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT InputLabelEntity : public Entity {
 public:
  ENTITY_ID(InputLabelEntity, "input param")
  using Entity::Entity;
  InputLabelEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT OutputEntity : public Entity {
 public:
  ENTITY_ID(OutputEntity, "output param")
  using Entity::Entity;
  OutputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT OutputLabelEntity : public Entity {
 public:
  ENTITY_ID(OutputLabelEntity, "output param")
  using Entity::Entity;
  OutputLabelEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT ComputeNodeEntity : public Entity {
 public:
  ENTITY_ID(ComputeNodeEntity, "compute")
  using Entity::Entity;
  ComputeNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

}

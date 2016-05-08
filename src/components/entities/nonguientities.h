#pragma once
#include <base/objectmodel/entity.h>
#include <components/entities/entities_export.h>
#include <components/computes/plugtype.h>
#include <components/entities/entityids.h>

namespace ngs {

class ENTITIES_EXPORT BaseNamespaceEntity : public Entity {
 public:
  ENTITY_ID(BaseNamespaceEntity, "namespace")
  using Entity::Entity; // not supported on msvc 2013
  BaseNamespaceEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT BaseAppEntity : public Entity {
 public:
  ENTITY_ID(BaseAppEntity, "base app")
  using Entity::Entity;
  BaseAppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT BaseLinkEntity : public Entity {
 public:
  ENTITY_ID(BaseLinkEntity, "base link head")
  using Entity::Entity;
  BaseLinkEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT BaseGroupNodeEntity : public Entity {
 public:
  ENTITY_ID(BaseGroupNodeEntity, "base group")
  using Entity::Entity;
  BaseGroupNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT BaseDotNodeEntity : public Entity {
 public:
  ENTITY_ID(BaseDotNodeEntity, "base dot")
  using Entity::Entity;
  BaseDotNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT BaseInputNodeEntity : public Entity {
 public:
  ENTITY_ID(BaseInputNodeEntity, "base input")
  using Entity::Entity;
  BaseInputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT BaseOutputNodeEntity : public Entity {
 public:
  ENTITY_ID(BaseOutputNodeEntity, "base output")
  using Entity::Entity;
  BaseOutputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT BaseMockNodeEntity : public Entity {
 public:
  ENTITY_ID(BaseMockNodeEntity, "base mock")
  using Entity::Entity;
  BaseMockNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT BaseInputEntity : public Entity {
 public:
  ENTITY_ID(BaseInputEntity, "base input param")
  using Entity::Entity;
  BaseInputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT BaseOutputEntity : public Entity {
 public:
  ENTITY_ID(BaseOutputEntity, "base output param")
  using Entity::Entity;
  BaseOutputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

class ENTITIES_EXPORT BaseComputeNodeEntity : public Entity {
 public:
  ENTITY_ID(BaseComputeNodeEntity, "base compute")
  using Entity::Entity;
  BaseComputeNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals();
};

}

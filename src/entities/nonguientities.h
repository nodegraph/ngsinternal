#pragma once
#include <base/objectmodel/entity.h>
#include <entities/entities_export.h>
#include <entities/entityids.h>

namespace ngs {

enum class ParamType;

class ENTITIES_EXPORT BaseNamespaceEntity : public Entity {
 public:
  ENTITY_ID(BaseNamespaceEntity, "namespace")
  BaseNamespaceEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT BaseAppEntity : public Entity {
 public:
  ENTITY_ID(BaseAppEntity, "base app")
  BaseAppEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT BaseLinkEntity : public Entity {
 public:
  ENTITY_ID(BaseLinkEntity, "base link head")
  BaseLinkEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT BaseGroupNodeEntity : public Entity {
 public:
  ENTITY_ID(BaseGroupNodeEntity, "base group")
  BaseGroupNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT BaseDotNodeEntity : public Entity {
 public:
  ENTITY_ID(BaseDotNodeEntity, "base dot")
  BaseDotNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT BaseInputNodeEntity : public Entity {
 public:
  ENTITY_ID(BaseInputNodeEntity, "base input")
  BaseInputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT BaseOutputNodeEntity : public Entity {
 public:
  ENTITY_ID(BaseOutputNodeEntity, "base output")
  BaseOutputNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT BaseMockNodeEntity : public Entity {
 public:
  ENTITY_ID(BaseMockNodeEntity, "base mock")
  BaseMockNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

class ENTITIES_EXPORT BaseInputEntity : public Entity {
 public:
  ENTITY_ID(BaseInputEntity, "base input param")
  BaseInputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
  virtual void set_param_type(ParamType param_type);
};

class ENTITIES_EXPORT BaseOutputEntity : public Entity {
 public:
  ENTITY_ID(BaseOutputEntity, "base output param")
  BaseOutputEntity(Entity* parent, const std::string& name):Entity(parent, name){}
  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
};

//class ENTITIES_EXPORT BaseScriptNodeEntity : public Entity {
// public:
//  ENTITY_ID(BaseScriptNodeEntity, "base compute")
//  BaseScriptNodeEntity(Entity* parent, const std::string& name):Entity(parent, name){}
//  virtual void create_internals(const std::vector<size_t>& ids = std::vector<size_t>());
//};

}

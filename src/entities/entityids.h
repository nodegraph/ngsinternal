#pragma once
#include <entities/entities_export.h>
#include <string>

namespace ngs {

// These macros should be used in class declarations.
// These IDs are used heavily in serialization and identifying
// components in Entities.
// Entities always implement the Entity class interface.
#define ENTITY_ID(derived, derived_name)\
  static EntityIID kIID()\
  { return EntityIID::kIEntity; }\
  virtual EntityIID get_iid() const\
  { return kIID(); }\
  \
  static EntityDID kDID()\
  { return EntityDID::k##derived; }\
  virtual EntityDID get_did() const\
  { return kDID(); }\
  \
  static const char* kDIDName()\
  { return derived_name; }\
  virtual const char* get_did_name() const\
  { return kDIDName(); }\


// Entity Interface IDs.
enum class EntityIID : size_t {
  kIInvalidEntity,
  kIEntity,
};

#define ENTITY_ENTRY1(NAME) k##NAME,
#define ENTITY_ENTRY2(NAME, VALUE) k##NAME = VALUE,

#define ENTITY_ENTRIES()\
ENTITY_ENTRY1(InvalidEntity)\
/* ------------------------------ */\
/* Non Gui Based Entities. */\
/* ------------------------------ */\
ENTITY_ENTRY1(BaseNamespaceEntity)\
/* App. */\
ENTITY_ENTRY1(BaseAppEntity)\
/* Links. */\
ENTITY_ENTRY1(BaseLinkEntity)\
ENTITY_ENTRY1(BaseInputEntity)\
ENTITY_ENTRY1(BaseOutputEntity)\
/* Group Node. */\
ENTITY_ENTRY1(BaseGroupNodeEntity)\
ENTITY_ENTRY1(BaseInputNodeEntity)\
ENTITY_ENTRY1(BaseOutputNodeEntity)\
/* Dot Node. */\
ENTITY_ENTRY1(BaseDotNodeEntity)\
/* Other Nodes. */\
ENTITY_ENTRY1(BaseMockNodeEntity)\
/*ENTITY_ENTRY1(BaseScriptNodeEntity)*/\
/* ------------------------------ */\
/* Gui Based Entities. */\
/* ------------------------------ */\
/* App. */\
ENTITY_ENTRY1(AppEntity)\
ENTITY_ENTRY1(QMLAppEntity)\
ENTITY_ENTRY1(QtAppEntity)\
/* Links. */\
ENTITY_ENTRY1(LinkEntity)\
ENTITY_ENTRY1(InputEntity)\
ENTITY_ENTRY1(InputLabelEntity)\
ENTITY_ENTRY1(OutputEntity)\
ENTITY_ENTRY1(OutputLabelEntity)\
/* Group Node. */\
ENTITY_ENTRY1(GroupNodeEntity)\
ENTITY_ENTRY1(ScriptGroupNodeEntity)\
ENTITY_ENTRY1(BrowserGroupNodeEntity)\
ENTITY_ENTRY1(FirebaseGroupNodeEntity)\
ENTITY_ENTRY1(MQTTGroupNodeEntity)\
ENTITY_ENTRY1(DataNodeEntity)\
ENTITY_ENTRY1(InputNodeEntity)\
ENTITY_ENTRY1(OutputNodeEntity)\
/* Dot Node. */\
ENTITY_ENTRY1(DotNodeEntity)\
/* Compute Nodes. */\
ENTITY_ENTRY1(ComputeNodeEntity)\
/* Macro Node. */\
ENTITY_ENTRY1(UserMacroNodeEntity)\
ENTITY_ENTRY1(AppMacroNodeEntity)\


enum class EntityDID: size_t {
  ENTITY_ENTRIES()
};

// Returns the derived class name as a string.
ENTITIES_EXPORT const char* get_entity_did_name(EntityDID did);

// Returns a more user friendly name for the derived class.
ENTITIES_EXPORT std::string get_entity_user_did_name(EntityDID did);
}

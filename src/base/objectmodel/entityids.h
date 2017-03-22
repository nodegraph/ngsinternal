#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/utils/enumutil.h>
#include <string>

namespace ngs {

// Names for entities which act like folders.
OBJECTMODEL_EXPORT extern const char* kInputsFolderName;
OBJECTMODEL_EXPORT extern const char* kOutputsFolderName;
OBJECTMODEL_EXPORT extern const char* kLinksFolderName;

// Names for special entities in groups.
OBJECTMODEL_EXPORT extern const char* kMainInputNodeName;
OBJECTMODEL_EXPORT extern const char* kMainOutputNodeName;

// These macros should be used in class declarations.
// These IDs are used heavily in serialization and identifying
// components in Entities.
// Entities always implement the Entity class interface.
#define ENTITY_ID(derived)\
  static EntityIID kIID()\
  { return EntityIID::kIEntity; }\
  virtual EntityIID get_iid() const\
  { return kIID(); }\
  \
  static EntityDID kDID()\
  { return EntityDID::k##derived; }\
  virtual EntityDID get_did() const\
  { return kDID(); }\


// Entity Interface IDs.
enum class EntityIID : size_t {
  kIInvalidEntity = 0,
  kIEntity = 1,
};

#define ENTITY_ENTRY1(NAME) k##NAME,
#define ENTITY_ENTRY2(NAME, VALUE) k##NAME = VALUE,

#define ENTITY_ENTRIES()\
ENTITY_ENTRY2(InvalidEntity, 0)\
/* ------------------------------ */\
/* Non Gui Based Entities. */\
/* ------------------------------ */\
ENTITY_ENTRY2(BaseNamespaceEntity, 1)\
/* App. */\
ENTITY_ENTRY2(BaseAppEntity, 2)\
/* Links. */\
ENTITY_ENTRY2(BaseLinkEntity, 3)\
ENTITY_ENTRY2(BaseInputEntity, 4)\
ENTITY_ENTRY2(BaseOutputEntity, 5)\
/* Group Node. */\
ENTITY_ENTRY2(BaseGroupNodeEntity, 6)\
ENTITY_ENTRY2(BaseInputNodeEntity, 7)\
ENTITY_ENTRY2(BaseOutputNodeEntity, 8)\
/* Dot Node. */\
ENTITY_ENTRY2(BaseDotNodeEntity, 9)\
/* Other Nodes. */\
ENTITY_ENTRY2(BaseMockNodeEntity, 10)\
/* ------------------------------ */\
/* Gui Based Entities. */\
/* ------------------------------ */\
/* App. */\
ENTITY_ENTRY2(AppEntity, 11)\
ENTITY_ENTRY2(QMLAppEntity, 12)\
ENTITY_ENTRY2(QtAppEntity, 13)\
/* Links. */\
ENTITY_ENTRY2(LinkEntity, 14)\
ENTITY_ENTRY2(InputEntity, 15)\
ENTITY_ENTRY2(InputLabelEntity, 16)\
ENTITY_ENTRY2(OutputEntity, 17)\
ENTITY_ENTRY2(OutputLabelEntity, 18)\
/* Group Nodes. */\
ENTITY_ENTRY2(GroupNodeEntity, 19)\
ENTITY_ENTRY2(IfGroupNodeEntity, 20)\
ENTITY_ENTRY2(WhileGroupNodeEntity, 21)\
ENTITY_ENTRY2(BrowserGroupNodeEntity, 22)\
ENTITY_ENTRY2(MQTTGroupNodeEntity, 23)\
/* Data Nodes. */\
ENTITY_ENTRY2(DataNodeEntity, 24)\
/* Input Nodes. */\
ENTITY_ENTRY2(InputNodeEntity, 25)\
/* Output Nodes. */\
ENTITY_ENTRY2(OutputNodeEntity, 26)\
/* Dot Node. */\
ENTITY_ENTRY2(DotNodeEntity, 27)\
/* Wait Nodes. */\
ENTITY_ENTRY2(WaitNodeEntity, 28)\
/* Compute Nodes. */\
ENTITY_ENTRY2(ComputeNodeEntity, 29)\
/* Macro Node. */\
ENTITY_ENTRY2(PublicMacroNodeEntity, 30)\
ENTITY_ENTRY2(PrivateMacroNodeEntity, 34)\
ENTITY_ENTRY2(AppMacroNodeEntity, 31)\
/* Password Nodes. */\
ENTITY_ENTRY2(PasswordInputNodeEntity, 32)\
ENTITY_ENTRY2(PasswordDataNodeEntity, 33)\

// IDs currently in use: [0-34]

enum class EntityDID: size_t {
  ENTITY_ENTRIES()
};

// Returns the derived class name as a string.
OBJECTMODEL_EXPORT const char* get_entity_did_name(EntityDID did);

// Returns a more user friendly name for the derived class.
OBJECTMODEL_EXPORT std::string get_entity_user_did_name(EntityDID did);
}


namespace std {
  template <> struct hash<ngs::EntityDID> {
    size_t operator()(const ngs::EntityDID& x) const {
     return ngs::to_underlying(x);
    }
  };
  template <> struct hash<ngs::EntityIID> {
    size_t operator()(const ngs::EntityIID& x) const {
     return ngs::to_underlying(x);
    }
  };
}

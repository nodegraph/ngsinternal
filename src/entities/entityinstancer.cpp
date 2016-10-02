#include <base/memoryallocator/taggednew.h>
#include <entities/entityinstancer.h>
#include <entities/guientities.h>
#include <entities/nonguientities.h>
#include <entities/entityids.h>

namespace ngs {

class InvalidEntity: public Entity {
 public:
  ENTITY_ID(InvalidEntity, "invalid")
  InvalidEntity(Entity* parent, const std::string& name): Entity(parent, name){}
};

//#define ENTITY_CASES(case_statement)\
//    /* ------------------------------*/\
//    /* Non Gui Based Entities.       */\
//    /* ------------------------------*/\
//    case_statement(BaseNamespaceEntity)\
//    /* App. */\
//    case_statement(BaseAppEntity)\
//    /* Links. */\
//    case_statement(BaseLinkEntity)\
//    case_statement(BaseInputEntity)\
//    case_statement(BaseOutputEntity)\
//    /* Group Node. */\
//    case_statement(BaseGroupNodeEntity)\
//    case_statement(BaseInputNodeEntity)\
//    case_statement(BaseOutputNodeEntity)\
//    /* Dot Node. */\
//    case_statement(BaseDotNodeEntity)\
//    /* Other Nodes. */\
//    case_statement(BaseMockNodeEntity)\
//    case_statement(BaseScriptNodeEntity)\
//    /* ------------------------------*/\
//    /* Gui Based Entities.       */\
//    /* ------------------------------*/\
//    /* App. */\
//    case_statement(AppEntity)\
//    case_statement(QMLAppEntity)\
//    case_statement(QtAppEntity)\
//    /* Links. */\
//    case_statement(LinkEntity)\
//    case_statement(InputEntity)\
//    case_statement(InputLabelEntity)\
//    case_statement(OutputEntity)\
//    case_statement(OutputLabelEntity)\
//    /* Group Node. */\
//    case_statement(GroupNodeEntity)\
//    case_statement(InputNodeEntity)\
//    case_statement(OutputNodeEntity)\
//    /* Dot Node. */\
//    case_statement(DotNodeEntity)\
//    /* Compute Nodes. */\
//    case_statement(ComputeNodeEntity)
////    case_statement(MockNodeEntity)\
////    case_statement(ScriptNodeEntity)\
////    case_statement(OpenBrowserNodeEntity)\
////    case_statement(CloseBrowserNodeEntity)\
////    case_statement(CreateSetFromValuesNodeEntity)\
////    case_statement(CreateSetFromTypeNodeEntity)\
//    /* Invalids. */\
//    case kInvalidEntity:\
//    case kNumEntityImplementationIDs:\
//      break;

#define INST_CASE(E) case k##E: return new_ff E(parent, name);

Entity* EntityInstancer::instance(Entity* parent, const std::string& name, EntityDID did) const {
#undef ENTITY_ENTRY1
#undef ENTITY_ENTRY2
#define ENTITY_ENTRY1(NAME) case EntityDID::k##NAME: return new_ff NAME(parent, name);
#define ENTITY_ENTRY2(NAME, VALUE) ENTITY_ENTRY1(NAME)
  switch (did) {
    ENTITY_ENTRIES();
  }

//  EntityDID d = static_cast<EntityDID>(did);
//  switch (d) {
//    ENTITY_CASES(INST_CASE)
//  }
  std::cerr << "Error: unexpected entity type found during load.\n";
  assert(false);
  return NULL;
}

#define IID_CASE(E) case k##E: return E::kIID();

EntityIID EntityInstancer::get_iid(EntityDID did) const {
#undef ENTITY_ENTRY1
#undef ENTITY_ENTRY2
#define ENTITY_ENTRY1(NAME) case EntityDID::k##NAME: return NAME::kIID();
#define ENTITY_ENTRY2(NAME, VALUE) ENTITY_ENTRY1(NAME)
  switch (did) {
    ENTITY_ENTRIES();
  }
  std::cerr << "Error: unexpected entity type found during load.\n";
  assert(false);
  return EntityIID::kIInvalidEntity;
}

#define NAME_CASE(E) case k##E: return E::kDIDName();

const char* EntityInstancer::get_name_for_did(EntityDID did) const {
#undef ENTITY_ENTRY1
#undef ENTITY_ENTRY2
#define ENTITY_ENTRY1(NAME) case EntityDID::k##NAME: return NAME::kDIDName();
#define ENTITY_ENTRY2(NAME, VALUE) ENTITY_ENTRY1(NAME)
  switch (did) {
    ENTITY_ENTRIES()
  }
  std::cerr << "Error: unexpected entity type found during load.\n";
  assert(false);
  return "unknown did name";
}

}

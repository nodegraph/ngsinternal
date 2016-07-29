#include <base/memoryallocator/taggednew.h>
#include <components/entities/entityinstancer.h>
#include <components/entities/guientities.h>
#include <components/entities/nonguientities.h>

namespace ngs {

#define ENTITY_CASES(case_statement)\
    /* ------------------------------*/\
    /* Non Gui Based Entities.       */\
    /* ------------------------------*/\
    case_statement(BaseNamespaceEntity)\
    /* App. */\
    case_statement(BaseAppEntity)\
    /* Links. */\
    case_statement(BaseLinkEntity)\
    case_statement(BaseInputEntity)\
    case_statement(BaseOutputEntity)\
    /* Group Node. */\
    case_statement(BaseGroupNodeEntity)\
    case_statement(BaseInputNodeEntity)\
    case_statement(BaseOutputNodeEntity)\
    /* Dot Node. */\
    case_statement(BaseDotNodeEntity)\
    /* Other Nodes. */\
    case_statement(BaseMockNodeEntity)\
    case_statement(BaseComputeNodeEntity)\
    /* ------------------------------*/\
    /* Gui Based Entities.       */\
    /* ------------------------------*/\
    /* App. */\
    case_statement(AppEntity)\
    case_statement(QMLAppEntity)\
    case_statement(QtAppEntity)\
    /* Links. */\
    case_statement(LinkEntity)\
    case_statement(InputEntity)\
    case_statement(InputLabelEntity)\
    case_statement(OutputEntity)\
    case_statement(OutputLabelEntity)\
    /* Group Node. */\
    case_statement(GroupNodeEntity)\
    case_statement(InputNodeEntity)\
    case_statement(OutputNodeEntity)\
    /* Dot Node. */\
    case_statement(DotNodeEntity)\
    /* Other Nodes. */\
    case_statement(MockNodeEntity)\
    case_statement(ComputeNodeEntity)\
    case_statement(OpenBrowserNodeEntity)\
    case_statement(CloseBrowserNodeEntity)\
    /* Invalids. */\
    case kInvalidEntity:\
    case kNumEntityImplementationIDs:\
      break;

#define INST_CASE(E) case k##E: return new_ff E(parent, name);

Entity* EntityInstancer::instance(Entity* parent, const std::string& name, size_t did) const {
  EntityDID d = static_cast<EntityDID>(did);
  switch (d) {
    ENTITY_CASES(INST_CASE)
  }
  std::cerr << "Error: unexpected entity type found during load.\n";
  assert(false);
  return NULL;
}

#define IID_CASE(E) case k##E: return E::kIID();

size_t EntityInstancer::get_iid(size_t did) const {
  EntityDID d = static_cast<EntityDID>(did);
  switch (d) {
    ENTITY_CASES(IID_CASE)
  }
  std::cerr << "Error: unexpected entity type found during load.\n";
  assert(false);
  return -1;
}

#define NAME_CASE(E) case k##E: return E::kDIDName();

const char* EntityInstancer::get_did_name(size_t did) const {
  EntityDID d = static_cast<EntityDID>(did);
  switch (d) {
    ENTITY_CASES(NAME_CASE)
  }
  std::cerr << "Error: unexpected entity type found during load.\n";
  assert(false);
  return "unknown did name";
}

}

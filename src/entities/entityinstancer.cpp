#include <base/memoryallocator/taggednew.h>
#include <entities/entityinstancer.h>
#include <entities/guientities.h>
#include <entities/nonguientities.h>
#include <base/objectmodel/entityids.h>

namespace ngs {

class InvalidEntity: public Entity {
 public:
  ENTITY_ID(InvalidEntity, "invalid")
  InvalidEntity(Entity* parent, const std::string& name): Entity(parent, name){}
};

Entity* EntityInstancer::instance(Entity* parent, EntityDID did, const std::string& name) const {
  std::string entity_name;
  if (name.empty()) {
    entity_name = get_entity_user_did_name(did);

  } else {
    entity_name = name;
  }

#undef ENTITY_ENTRY1
#undef ENTITY_ENTRY2
#define ENTITY_ENTRY1(NAME) case EntityDID::k##NAME: return new_ff NAME(parent, entity_name);
#define ENTITY_ENTRY2(NAME, VALUE) ENTITY_ENTRY1(NAME)
  switch (did) {
    ENTITY_ENTRIES();
  }
  std::cerr << "Error: unexpected entity type found during load.\n";
  assert(false);
  return NULL;
}

EntityIID EntityInstancer::get_iid_for_did(EntityDID did) const {
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

}

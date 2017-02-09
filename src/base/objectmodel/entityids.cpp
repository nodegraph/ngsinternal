#include <base/objectmodel/entityids.h>
#include <base/objectmodel/componentids.h>

namespace ngs {

const char* get_entity_did_name(EntityDID did) {
  #undef ENTITY_ENTRY1
  #undef ENTITY_ENTRY2
  #define ENTITY_ENTRY1(NAME) case EntityDID::k##NAME: return #NAME;
  #define ENTITY_ENTRY2(NAME, VALUE) ENTITY_ENTRY1(NAME)
  switch (did) {
    ENTITY_ENTRIES()
  }
  return "unknown entity did type";
}

std::string get_entity_user_did_name(EntityDID did) {
  std::string name = get_entity_did_name(did);
  return get_user_friendly_name(name);
}

}

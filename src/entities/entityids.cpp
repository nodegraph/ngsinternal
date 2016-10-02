#include <entities/entityids.h>

namespace ngs {

const char* get_entity_did_name(EntityDID did) {
  #undef ENTITY_ENTRY1
  #undef ENTITY_ENTRY2
  #define ENTITY_ENTRY1(NAME) case ComponentDID::k##NAME: return #NAME;
  #define ENTITY_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  switch (did) {
    ENTITY_ENTRIES()
  }
  return "unknown entity did type";
}

}

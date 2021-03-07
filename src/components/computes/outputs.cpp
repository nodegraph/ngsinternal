#include <components/computes/outputs.h>

namespace ngs {

// -------------------------------------------------------------------------------------------------------------------
// Outputs.
// -------------------------------------------------------------------------------------------------------------------

Outputs::Outputs(Entity* entity):
    Flux<OutputTraits>(entity){
}

Outputs::~Outputs() {
}

}

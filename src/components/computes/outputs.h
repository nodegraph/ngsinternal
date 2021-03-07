#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/flux.h>

namespace ngs {

//--------------------------------------------------------------------------
// The Outputs class adds additional output specific methods onto the base Flux.
//--------------------------------------------------------------------------

class COMPUTES_EXPORT Outputs: public Flux<OutputTraits> {
 public:
  Outputs(Entity* entity);
  virtual ~Outputs();
};

}

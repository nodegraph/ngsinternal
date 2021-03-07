#pragma once
#include <base/objectmodel/objectmodel_export.h>

namespace ngs {

class SimpleSaver;
class SimpleLoader;

// The interface for deps which are combined with string paths
// for the purposes of serialization.
class OBJECTMODEL_EXPORT PathedDep {
 public:
  virtual ~PathedDep() {}

  virtual void save(SimpleSaver& saver) const {}
  virtual void load(SimpleLoader& loader) {}
  virtual void update_wire() {}
  virtual void bake_path() const {}
};


}

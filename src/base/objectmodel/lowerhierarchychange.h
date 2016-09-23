#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

namespace ngs {

// This component represents the cumulative dirty state of the entity
// tree below the entity to which this component belongs.
// For example:
//    A
//   / \
//  B   C
// The lower hierarchy change component at entity A will be dirty
// under the following conditions.
// 1) Entity A was just renamed.
// 2) A child entity was just added. (eg B or C)
// 3) A child entity was just removed. (eg D or E)
// 4) A child entity was just renamed. (eg D was renamed to B)
//
// Note implementation wise there are some hooks in the Entity class
// where when child entities are added or removed the lower hierarchy change
// component gets dirtied.

class OBJECTMODEL_EXPORT LowerHierarchyChange : public Component {
 public:

  COMPONENT_ID(LowerHierarchyChange, LowerHierarchyChange);

  LowerHierarchyChange(Entity* entity);
  virtual ~LowerHierarchyChange();

  // Our state.
  virtual void initialize_fixed_deps();
  virtual void update_state();

 private:
  // Our dynamic deps. These are not serialized and gathered at runtime.
  DepUSet<LowerHierarchyChange> _child_changes;
};

}

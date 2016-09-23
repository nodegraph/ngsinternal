#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>


namespace ngs {

// Note: This component is the opposite of the LowerHierarchyChange Component.
// This component represents the cumulative dirty state of the parent
// entities above the entity to which this component belongs.
// For example:
//    A
//   / \
//  B   C
// The upper hierarchy change component at entity B will be dirty
// under the following conditions.
// 1) Entity B was just renamed.
// 2) A parent entity was just added.
// 3) Entity equal to or above us acquired a new child above us. (eg A got a got a child C.)
// 3) A parent entity was just removed. (eg A had a parent X before.)
// 4) A parent entity was just renamed. (eg A was named Y before.)
//
// Note implementation wise there are some hooks in the Entity class
// where when child entities are added or removed the lower hierarchy change
// component gets dirtied.

// A use case for the UpperHierarchyChange component is
// when an entity is renamed (for example a group),
// all the child entities' paths will change.
// These paths may be displayed in the gui .. etc, requiring an update.


class OBJECTMODEL_EXPORT UpperHierarchyChange : public Component {
 public:

  COMPONENT_ID(UpperHierarchyChange, UpperHierarchyChange);

  UpperHierarchyChange(Entity* entity);
  virtual ~UpperHierarchyChange();

  // Our state.
//  virtual void initialize_fixed_deps();
//  virtual void update_state();

 private:

  // Our fixed deps.
  Dep<UpperHierarchyChange> _parent_change;
};

}

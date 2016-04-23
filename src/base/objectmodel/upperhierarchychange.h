#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/component.h>
#include <components/entities/componentids.h>
#include <base/objectmodel/dep.h>


namespace ngs {

// This class is represents changes that happen to an Entity.
// These changes are just the addition and removal of child entities.
// Each EntityChanges has a dependency on it's parent entity's
// EntityChange component. This is because entities are allowed
// to be renamed. When an entity is renamed (for example a group),
// all the child entities' paths will change.
// These paths may be displayed in the gui .. etc, requiring an update.

class UpperHierarchyChange;

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

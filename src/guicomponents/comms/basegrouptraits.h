#pragma once
#include <base/objectmodel/component.h>
#include <entities/componentids.h>

namespace ngs {

enum class GroupType {
  DataGroup,
  WebGroup,
};

class BaseGroupTraits: public Component {
 public:
  COMPONENT_ID(BaseGroupTraits, InvalidComponent);
  BaseGroupTraits(Entity* entity, ComponentDID did): Component(entity, kIID(), did){}
  virtual ~BaseGroupTraits(){}

  virtual GroupType get_group_type() const = 0;

  virtual void on_enter() = 0; // called when we enter this group
  virtual void on_exit() = 0; // called when we exit this group

//  virtual bool node_type_is_permitted(EntityDID did) const = 0;
//  virtual const std::unordered_set<EntityDID>& get_permitted_node_types() = 0;

 private:

};

}

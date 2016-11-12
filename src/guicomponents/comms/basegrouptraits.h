//#pragma once
//#include <base/objectmodel/component.h>
//#include <entities/componentids.h>
//
//namespace ngs {
//
//enum class GroupType {
//  DataGroup,
//  WebGroup,
//  ExpGroup,
//};
//
//class BaseGroupTraits: public Component {
// public:
//  COMPONENT_ID(BaseGroupTraits, InvalidComponent);
//  BaseGroupTraits(Entity* entity, ComponentDID did): Component(entity, kIID(), did){}
//  virtual ~BaseGroupTraits(){}
//
//  virtual GroupType get_group_type() const = 0;
//
//  virtual bool ok_to_dive() const {return true;}
//  virtual void on_enter() {} // called when we enter this group
//  virtual void on_exit() {} // called when we exit this group
//  virtual void on_clean() {}
//
////  virtual bool node_type_is_permitted(EntityDID did) const = 0;
////  virtual const std::unordered_set<EntityDID>& get_permitted_node_types() = 0;
//
// private:
//
//};
//
//}

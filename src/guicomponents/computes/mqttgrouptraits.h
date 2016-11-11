//#pragma once
//#include <guicomponents/computes/guicomputes_export.h>
//#include <base/objectmodel/dep.h>
//#include <entities/componentids.h>
//#include <guicomponents/comms/basegrouptraits.h>
//
//namespace ngs {
//
//class MQTTWorker;
//class Inputs;
//
//class GUICOMPUTES_EXPORT MQTTGroupTraits: public BaseGroupTraits {
// public:
//  COMPONENT_ID(BaseGroupTraits, MQTTGroupTraits);
//  MQTTGroupTraits(Entity* entity);
//  virtual ~MQTTGroupTraits();
//
//  virtual GroupType get_group_type() const {return GroupType::WebGroup;}
//
//  virtual bool ok_to_dive() const {return false;}
//  virtual void on_enter(); // called when we enter this group
//  virtual void on_exit(); // called when we exit this group
//  virtual void on_clean();
//
////  virtual bool node_type_is_permitted(EntityDID did) const;
////  virtual const std::unordered_set<EntityDID>& get_permitted_node_types();
//
// private:
//  Dep<MQTTWorker> _worker;
//  Dep<Inputs> _inputs;
//};
//
//}

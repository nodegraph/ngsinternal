#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/dep.h>
#include <entities/componentids.h>
#include <guicomponents/comms/basegrouptraits.h>

namespace ngs {

class BrowserWorker;
class Inputs;

class COMMS_EXPORT FirebaseGroupTraits: public BaseGroupTraits {
 public:
  COMPONENT_ID(BaseGroupTraits, FirebaseGroupTraits);
  FirebaseGroupTraits(Entity* entity);
  virtual ~FirebaseGroupTraits();

  virtual GroupType get_group_type() const {return GroupType::WebGroup;}

  virtual bool ok_to_dive() const {return false;}
  virtual void on_enter(); // called when we enter this group
  virtual void on_exit(); // called when we exit this group
  virtual void on_clean();

//  virtual bool node_type_is_permitted(EntityDID did) const;
//  virtual const std::unordered_set<EntityDID>& get_permitted_node_types();

 private:
  Dep<BrowserWorker> _web_worker;
  Dep<Inputs> _inputs;
};

}

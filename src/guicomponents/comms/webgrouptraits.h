#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/dep.h>
#include <entities/componentids.h>
#include <guicomponents/comms/basegrouptraits.h>

namespace ngs {

class WebWorker;

class COMMS_EXPORT WebGroupTraits: public BaseGroupTraits {
 public:
  COMPONENT_ID(BaseGroupTraits, WebGroupTraits);
  WebGroupTraits(Entity* entity);
  virtual ~WebGroupTraits();

  virtual GroupType get_group_type() const {return GroupType::WebGroup;}

  virtual void on_enter(); // called when we enter this group
  virtual void on_exit(); // called when we exit this group

//  virtual bool node_type_is_permitted(EntityDID did) const;
//  virtual const std::unordered_set<EntityDID>& get_permitted_node_types();

 private:
  Dep<WebWorker> _web_worker;

};

}

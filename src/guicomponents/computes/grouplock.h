#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class Inputs;

class GUICOMPUTES_EXPORT GroupLock: public Component  {
 public:
  COMPONENT_ID(GroupLock, GroupLock);
  GroupLock(Entity* entity, ComponentIID iid = kIID(), ComponentDID did = kDID());
  virtual ~GroupLock();

  virtual bool get_lock_setting() const {return false;}
  virtual void set_lock_setting(bool lock) {}

 protected:
  Dep<Inputs> _inputs;
};

}

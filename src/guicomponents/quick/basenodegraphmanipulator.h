#pragma once
#include <guicomponents/quick/quick_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>

namespace ngs {

class BaseNodeGraphManipulator: public Component {
 public:
  COMPONENT_ID(BaseNodeGraphManipulator, InvalidComponent);

  BaseNodeGraphManipulator(Entity* entity, ComponentDID did):Component(entity, kIID(), did){}
  virtual ~BaseNodeGraphManipulator(){}

  virtual void set_processing(Entity* entity) = 0;

};

}

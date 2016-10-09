#pragma once
#include <guicomponents/quick/quick_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>

namespace ngs {

class BaseVisualizeProcessing: public Component {
 public:
  COMPONENT_ID(BaseVisualizeProcessing, InvalidComponent);

  BaseVisualizeProcessing(Entity* entity, ComponentDID did):Component(entity, kIID(), did){}
  virtual ~BaseVisualizeProcessing(){}

  virtual void set_processing(Entity* entity) = 0;

};

}

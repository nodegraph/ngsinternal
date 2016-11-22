#pragma once
#include <entities/entities_export.h>
#include <base/objectmodel/basefactory.h>

namespace ngs {

class Entity;
class BaseEntityInstancer;
class BaseComponentInstancer;

class ENTITIES_EXPORT Factory: public BaseFactory {
 public:

  COMPONENT_ID(BaseFactory, Factory);

  Factory(Entity* entity);
  virtual ~Factory();

//  virtual Entity* instance_compute_node(Entity* parent, ComponentDID compute_did, const std::string& name="") const;
//  virtual Entity* instance_input_node(Entity* parent, const QJsonValue& default_value, const std::string& name="") const;
};

}

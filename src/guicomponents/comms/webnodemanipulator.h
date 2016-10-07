#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

#include <QtCore/QVariantMap>

namespace ngs {

class BaseFactory;
class Entity;
class Compute;


class WebNodeManipulator: public Component {
 public:
  COMPONENT_ID(InvalidComponent, InvalidComponent);
  // Entity is assumed to be set to the app root.
  WebNodeManipulator(Entity* entity);

  void build_and_link_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);
  Entity* get_node() {return _node;}
  Compute* get_compute() {return _compute;}

 private:
  void build_compute_node(ComponentDID compute_did, const QVariantMap& chain_state);
  void link(Entity* downstream);

  Dep<BaseFactory> _factory;
  Entity* _node;
  Compute* _compute;
};


}

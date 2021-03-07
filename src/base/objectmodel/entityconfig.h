#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/component.h>


#include <QtCore/QJsonValue>

namespace ngs {

struct EntityConfig {
  EntityConfig() {
    visible = true;
    expose_plug = true;
    compute_did = ComponentDID::kInvalidComponent;
  }

  // Used by all entities to determine if gui components should be constructed.
  bool visible;

  // Used by plug entities to configure whether they are exposed.
  bool expose_plug;

  // Used by compute node entities to set the compute component type.
  ComponentDID compute_did;

  // Used by input compute entities to set the unconnected value.
  QJsonValue unconnected_value;
};

}

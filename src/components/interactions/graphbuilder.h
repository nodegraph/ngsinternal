#pragma once
#include <components/interactions/interactions_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <base/objectmodel/componentids.h>

namespace ngs {

class BaseFactory;
class BaseNodeGraphManipulator;

class INTERACTIONS_EXPORT GraphBuilder: public Component {
 public:

  COMPONENT_ID(GraphBuilder, GraphBuilder);

  GraphBuilder(Entity* entity);
  virtual ~GraphBuilder();

  // Graph Builders.
  virtual void build_test_graph();
  virtual void build_stress_graph();


 private:
  // Our fixed deps.
  Dep<BaseFactory> _factory;
  Dep<BaseNodeGraphManipulator> _manipulator;


};

}

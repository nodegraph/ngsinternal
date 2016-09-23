#pragma once
#include <components/interactions/interactions_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/dep.h>
#include <entities/componentids.h>

namespace ngs {

class BaseFactory;

class INTERACTIONS_EXPORT GraphBuilder: public Component {
 public:

  COMPONENT_ID(GraphBuilder, GraphBuilder);

  GraphBuilder(Entity* entity);
  virtual ~GraphBuilder();

  // Web Action Node Builders.
  virtual Entity* build_open_browser_node();
  virtual Entity* build_close_browser_node();
  virtual Entity* build_click_node(int set_index, int overlay_index, float rel_x, float rel_y);

  // Test Graph Builders.

  // Assumes that app_root is build with one group node as a child.
  virtual void build_test_graph();
  virtual void build_stress_graph();



 private:
  // Our fixed deps.
  Dep<BaseFactory> _factory;
};

}

#pragma once
#include <components/interactions/interactions_export.h>
#include <components/interactions/canvas.h>
#include <components/resources/resources.h>
#include <base/device/transforms/glmhelper.h>
#include <base/device/program/defaultuniforminfo.h>
#include <components/compshapes/nodeselection.h>
#include <map>
#include <set>
#include <vector>
#include <unordered_map>

namespace ngs {

class QuadPipelineSetup;
class TextPipelineSetup;

class NodeGroup;
class DeviceMemory;
class MouseInfo;
class KeyInfo;

class FontPack;

class GroupInteraction;
class CompShapeCollective;
class BaseFactory;


class INTERACTIONS_EXPORT ShapeCanvas: public Component {
 public:

  COMPONENT_ID(ShapeCanvas, ShapeCanvas);

  ShapeCanvas(Entity* entity);
  virtual ~ShapeCanvas();

  // Our state.
  virtual void gather_wires();
  virtual void update_state();
  virtual void initialize_gl();
  virtual void uninitialize_gl();
  virtual bool is_initialized_gl();

  // Draw.
  virtual void draw_gl();

  // Get pipeline.
  virtual QuadPipelineSetup* get_pipeline() {return _quad_pipeline;}

  // Group navigation.
  void surface();
  void surface_to_root();
  void dive(Entity* selected);
  void dive();
  const Dep<GroupInteraction>& get_current_interaction() const;


 private:

  // Group navigation management.
  void push_group(Entity* group);
  void pop_group();

  Entity* get_current_group() const;
  const Dep<CompShapeCollective>& get_current_shape_collective() const;

  // Our fixed deps.
  Dep<BaseFactory> _factory;
  Dep<Resources> _resources;
  Dep<NodeSelection> _ng_state;

  // Our dynamic deps. These are not serialized.
  Dep<GroupInteraction> _current_interaction;
  Dep<CompShapeCollective> _current_shape_collective;

  // Our drawing pipelines.
  QuadPipelineSetup* _quad_pipeline;
  TextPipelineSetup* _text_pipeline;

  // Draw settings.
  bool _clear_depth;
  bool _clear_color;

  // Our group stack.
  std::vector<Entity*> _group_stack;
};

}

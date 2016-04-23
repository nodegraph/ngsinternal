//#pragma once
//#include <components/interactions/interactions_export.h>
//#include <base/device/transforms/glmhelper.h>
//#include <base/objectmodel/component.h>
//#include <components/entities/componentids.h>
//#include <map>
//#include <set>
//#include <vector>
//
//namespace open_node_graph {
//
//class IDGenerator;
//class Interaction;
//class QuadPipelineSetup;
//
//class INTERACTIONS_EXPORT Canvas: public Component{
// public:
//
//  COMPONENT_ID(Canvas, Canvas);
//
//  Canvas(Entity* entity);
//  virtual ~Canvas();
//
//  // Device Initialization. Note we have GroupGeoLoader inside us which loads to device.
//  virtual void initialize_gl(){}
//  virtual void draw_gl(){}
//
//  // Draw Settings.
//  virtual void darken() {};
//  virtual void lighten() {};
//  virtual void clear_depth_on_draw(bool v) {_clear_depth = v;}
//  virtual void clear_color_on_draw(bool v) {_clear_color = v;}
//
//  virtual QuadPipelineSetup* get_pipeline() = 0;
//  virtual Interaction* get_current_group() = 0;
//
//  virtual void select_all() = 0;
//  virtual void frame_all() = 0;
//
// private:
//
//  // Draw settings.
//  bool _clear_depth;
//  bool _clear_color;
//};
//
//}

#pragma once
#include <components/compshapes/compshapes_export.h>
#include <components/compshapes/compshape.h>
#include <base/utils/polygon.h>


namespace ngs {

class Resources;
class NodeGraphSelection;
class Compute;

class COMPSHAPES_EXPORT LinkableShape: public CompShape {
 public:

  COMPONENT_ID(CompShape, LinkableShape);

  LinkableShape(Entity* entity, size_t did):CompShape(entity, did){}
  virtual ~LinkableShape(){}

  // Input and Output Ordering.
  virtual size_t get_input_order(const std::string& input_name) const = 0;
  virtual size_t get_output_order(const std::string& output_name) const = 0;

  // Num inputs and outputs.
  virtual size_t get_num_linkable_inputs() const = 0; // number of inputs with input plugs
  virtual size_t get_num_linkable_outputs() const = 0; // number of outputs with output plugs
  virtual size_t get_num_input_params() const = 0; // number of inputs without input plugs
  virtual size_t get_num_output_params() const = 0; // number of outputs without output plugs
  virtual size_t get_num_all_inputs() const = 0; // number of all inputs
  virtual size_t get_num_all_outputs() const = 0; // number of all outputs
};

}

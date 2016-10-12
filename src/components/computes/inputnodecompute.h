#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>


namespace ngs {

class COMPUTES_EXPORT InputNodeCompute: public Compute {
 public:
  COMPONENT_ID(Compute, InputNodeCompute);
  InputNodeCompute(Entity* entity);
  virtual ~InputNodeCompute();

  virtual void create_inputs_outputs();

  virtual void set_value(const QVariant& value);
  virtual QVariant get_value() const;

  // Our data is always set on us from the group.
  // So we don't need to serialize any extra state.
};

}

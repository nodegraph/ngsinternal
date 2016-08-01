#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

#include <QtCore/QObject>

namespace ngs {

class OutputCompute;

class COMPUTES_EXPORT InputCompute: public QObject, public Compute  {
  Q_OBJECT
 public:

  COMPONENT_ID(Compute, InputCompute);

  InputCompute(Entity* entity);
  virtual ~InputCompute();

  // Our state.
  virtual void update_state();

  // Our value.
  void set_value(QVariant& var);

  // Our dynamic output compute dependency.
  bool can_link_output_compute(const Dep<OutputCompute>& output) const;
  bool link_output_compute(Dep<OutputCompute>& output);
  void unlink_output_compute();
  Dep<OutputCompute> get_output_compute() const;

 private:

  // Our dynamic deps.
  Dep<OutputCompute> _output;

  // Our data.
  QVariant _param_data;
};


}

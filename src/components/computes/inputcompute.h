#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

#include <QtCore/QObject>

namespace ngs {

class OutputCompute;

enum ParamType {
  kQVariantMap,
  kQString,
  kQStringList,
  kInt,
  kFloat,
  kWrapType,
  kActionType,
  kDirection,
};


class COMPUTES_EXPORT InputCompute: public QObject, public Compute  {
  Q_OBJECT
 public:

  COMPONENT_ID(Compute, InputCompute);

  InputCompute(Entity* entity);
  virtual ~InputCompute();

  // Our state.
  virtual void update_state();

  // Our value.
  void set_value(const QVariant& value);

  // Our data type.
  void set_param_type(ParamType param_type);

  // Exposure.
  void set_exposed(bool exposed);
  bool is_exposed() const;

  // Our dynamic output compute dependency.
  bool can_link_output_compute(const Dep<OutputCompute>& output) const;
  bool link_output_compute(Dep<OutputCompute>& output);
  void unlink_output_compute();
  Dep<OutputCompute> get_output_compute() const;

  // Serialization.
  virtual void save(SimpleSaver& saver) const;
  virtual void load(SimpleLoader& loader);

 private:

  // Our dynamic deps.
  Dep<OutputCompute> _output;

  // Our data.
  // Usually stores the value directly.
  // If it has a string starting with "data." then it's
  // an expression which pulls a value from the data tree flowing through the graph.
  QVariant _param_value;

  // Our data type.
  ParamType _param_type;

  // Whether we are exposed in the node graph as a plug on a node.
  bool _exposed;
};


}

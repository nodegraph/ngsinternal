#pragma once
#include <base/objectmodel/component.h>
#include <components/computes/computes_export.h>
#include <components/entities/componentids.h>
#include <base/objectmodel/dep.h>
#include <string>

// QT
#include <QtCore/QVariant>

namespace ngs {

#ifndef  ong_computes_EXPORTS
#define TEMPLATE_EXPORT extern
#else
#define TEMPLATE_EXPORT 
#endif

class LowerHierarchyChange;
class InputCompute;

class COMPUTES_EXPORT Compute: public Component {
 public:

  COMPONENT_ID(Compute, InvalidComponent);

  Compute(Entity* entity, size_t derived_id);
  virtual ~Compute();

  // Our state.
  virtual void update_state();

  // Our compute type.
  const char* get_compute_type() {return "";}

  // Our input and output names.
  virtual const std::vector<std::string>& get_input_names(){return _input_names;}
  virtual const std::vector<std::string>& get_output_names(){return _output_names;}
  virtual size_t get_input_order(const std::string& input_name) const;
  virtual size_t get_output_order(const std::string& output_name) const;

  // Our results.
  virtual const QVariant& get_result(const std::string& name) const;

  // Helpers.
  static bool check_variant_is_bool_and_true(const QVariant& value, const std::string& message);
  static bool check_variant_is_list(const QVariant& value, const std::string& message);
  static bool check_variant_is_map(const QVariant& value, const std::string& message);

 protected:
  void configure();
  void set_result(const std::string& name, const QVariant& value);

  static const QVariant _empty_variant;

 protected:
  // Our fixed deps.
  Dep<LowerHierarchyChange> _lower_change;

  // Our dynamic deps. These are gathered and not saved.
  // Map from input plug names to their internal output plugs.
  std::unordered_map<std::string, Dep<InputCompute> > _inputs;

  // Our ordering.
  std::vector<std::string> _input_names;
  std::vector<std::string> _output_names;

  // Map from output plug names to results.
  // QVariantMap is a typedef for QMap<QString, QVariant>.
  QVariantMap _results;
};

}

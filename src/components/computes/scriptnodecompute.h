#pragma once
#include <components/computes/computes_export.h>
#include <components/computes/compute.h>

namespace ngs {


class COMPUTES_EXPORT ScriptNodeCompute: public Compute  {
 public:

  COMPONENT_ID(Compute, ScriptNodeCompute);

  ScriptNodeCompute(Entity* entity);
  virtual ~ScriptNodeCompute();

  // Our state.
  virtual void initialize_deps();
  virtual void uninitialize_deps();
  virtual void update_state();

  // Our compute type.
  virtual void set_compute_type(const std::string& compute_type);
  virtual const std::string& get_compute_type() const;

  // Call after the entity is fully built.
  virtual void configure();

//  // Serialization.
//  virtual void save(SimpleSaver& saver) const;
//  virtual void load(SimpleLoader& loader);

  // Compute setup overrides.
  virtual void update_input_names();
  virtual void update_output_names();

 protected:
  // Our flattened path as a variant.
  QVariant _flattened_path;

 private:
  std::string _compute_type;
  // Our params.
  QVariantMap _params;
};

}

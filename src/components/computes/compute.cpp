#include <components/computes/compute.h>
#include <components/computes/inputcompute.h>
#include <base/objectmodel/lowerhierarchychange.h>
#include <base/objectmodel/deploader.h>

namespace ngs {

const QVariant Compute::_empty_variant;

Compute::Compute(Entity* entity, size_t derived_id)
    : Component(entity, kIID(), derived_id),
      _lower_change(this) {
  get_dep_loader()->register_fixed_dep(_lower_change, ".");
}

Compute::~Compute() {
}

void Compute::update_state() {
//  if (dep_is_dirty(_lower_change)) {
    // Clear our old inputs.
    _inputs.clear();

    // Connect to our input plugs.
    Entity* inputs = has_entity("./inputs");
    if (inputs) {
      const Entity::NameToChildMap& children = inputs->get_children();
      for (auto &iter: children) {
        Dep<InputCompute> dep = get_dep<InputCompute>(iter.second);
        if (dep) {
          _inputs.insert({iter.first, dep});
          // Because we just added a new connection in our cleaning state method,
          // we need to make sure that it is clean.
          dep->clean();
        }
      }
    }
//  }

  // We are now connected to our input values.
  // Derived classes now perform their compute.
}

const QVariantMap& Compute::get_results() const {
  return _results;
}

const QVariant& Compute::get_result(const std::string& name) const{
  if (!_results.count(name.c_str())) {
    return _empty_variant;
  }
  return _results[name.c_str()];
}

void Compute::set_result(const std::string& name, const QVariant& value) {
  _results.insert(name.c_str(), value);
}

bool Compute::check_variant_is_bool_and_true(const QVariant& value, const std::string& message) {
  if (value.userType() != QMetaType::Bool) {
    std::cerr << "Error: " << "variant is not a bool: " << message << "\n";
    return false;
  }
  if (!value.toBool()) {
    std::cerr << "Error: " << message << "\n";
    return false;
  }
  return true;
}

bool Compute::check_variant_is_list(const QVariant& value, const std::string& message) {
  // For some reason the value.userType() call returns 1119 which doesn't match
  // QMetaType::QVariantList or QMetaType::QStringList. So we use canConvert instead.
  if (value.canConvert(QMetaType::QVariantList)) {
    return true;
  }
  std::cerr << "Error: " << message << "\n";
  return false;
}

bool Compute::check_variant_is_map(const QVariant& value, const std::string& message) {
  if (value.userType() == QMetaType::QVariantMap) {
    return true;
  }
  std::cerr << "Error: " << message << "\n";
  return false;
}



}

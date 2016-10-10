#pragma once
#include <components/compshapes/compshapes_export.h>
#include <base/objectmodel/component.h>

namespace ngs {

template <bool INPUT>
class COMPSHAPES_EXPORT Topology: public Component {
 public:
  static ComponentIID kIID() {
    if (INPUT) {
      return ComponentIID::kIInputTopology;
    }
    return ComponentIID::kIOutputTopology;
  }
  static ComponentDID kDID() {
    if (INPUT) {
      return ComponentDID::kInputTopology;
    }
    return ComponentDID::kOutputTopology;
  }

  Topology(Entity* entity);
  virtual ~Topology();

  virtual void set_topology(const std::unordered_map<std::string, size_t>& ordering);
  virtual size_t get_exposed_index(const std::string& input_name) const;
  virtual size_t get_num_exposed() const;

 private:
  std::unordered_map<std::string, size_t> _exposed_ordering;

};

typedef Topology<true> InputTopology;
typedef Topology<false> OutputTopology;

}

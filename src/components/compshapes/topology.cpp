#include <components/compshapes/topology.h>


namespace ngs {

template <bool INPUT>
Topology<INPUT>::Topology(Entity* entity)
    : Component(entity, kIID(), kDID()) {
}

template <bool INPUT>
Topology<INPUT>::~Topology() {

}

template <bool INPUT>
void Topology<INPUT>::set_topology(const std::unordered_map<std::string, size_t>& ordering) {
  external();
  _exposed_ordering = ordering;
}

template <bool INPUT>
size_t Topology<INPUT>::get_exposed_index(const std::string& input_name) const {
  external();
  if (_exposed_ordering.count(input_name)) {
    return _exposed_ordering.at(input_name);
  }
  return -1;
}

template <bool INPUT>
size_t Topology<INPUT>::get_num_exposed() const {
  external();
  return _exposed_ordering.size();
}

// Explicit Template Instantiation.
template class Topology<true>;
template class Topology<false>;

}

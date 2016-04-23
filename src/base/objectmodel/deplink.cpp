#include <base/objectmodel/component.h>
#include <base/objectmodel/deplink.h>

namespace ngs {

DepLink::DepLink(Component* dependant, Component* dependency)
    : dependant(dependant),
      dependency(dependency),
      dependency_hash(dependency),
      iid(dependency->get_iid()){
  // The dependency_hash will be fixed for the lifetime of this object.
  assert(dependant);
  assert(dependency);
}
DepLink::~DepLink() {
  assert(dependency_hash);

  // If our dependant is destroyed, we have nothing to unregister.
  if (!dependant) {
    return;
  }

  // Otherwise.
  if (dependency) {
    // If we have a valid dependency, that means that we're connected to our dependency,
    // and our dependency is connected to us as a dependant.
    // In this case the dependency component has not been destroyed and we can use our
    // usual disconnect method.
    assert(dependency_hash == dependency);
    dependant->disconnect_from_dep(dependency_hash);
  } else {
    // Otherwise we are already disconnected from our dependency.
    // Our dependency may have been destroyed as well.
    // So we just need to remove the dep link, making sure to only use the Component
    // pointer as a number or index, not calling any methods on it.
    dependant->remove_dep_link(dependency_hash, iid);
  }
}

DepLinkHolder::~DepLinkHolder() {

}

Component* DepLinkHolder::get_component() const {
  const DepLinkPtr& link = get_link();
  if (link) {
    return link->dependency;
  }
  return NULL;
}

}

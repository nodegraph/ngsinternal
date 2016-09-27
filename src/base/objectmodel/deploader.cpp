#include <base/objectmodel/deploader.h>
#include <base/objectmodel/patheddeps.h>

namespace ngs {

DepLoader::DepLoader() {}

DepLoader::~DepLoader() {
  for (PathedDep* d: _fixed_deps) {
    delete_ff(d);
  }
  for (PathedDep* d: _dynamic_deps) {
    delete_ff(d);
  }
  for (PathedDep* d: _set_deps) {
    delete_ff(d);
  }
}

void DepLoader::update_fixed_wires() {
  for (PathedDep* d: _fixed_deps) {
    d->update_wire();
  }
}

void DepLoader::update_dynamic_wires() {
  for (PathedDep* d: _dynamic_deps) {
    d->update_wire();
  }
}

void DepLoader::save(SimpleSaver& saver) const {
  // The size of the dynamic deps is fixed by the class
  // when it register dynamic deps.
  for (PathedDep* d: _dynamic_deps) {
    d->save(saver);
  }
}
void DepLoader::load(SimpleLoader& loader) {
  // The size of the dynamic deps is fixed by the class
  // when it register dynamic deps.
  for (PathedDep* d: _dynamic_deps) {
    d->load(loader);
  }
}
void DepLoader::bake_paths() {
  // The size of the dynamic deps is fixed by the class
  // when it register dynamic deps.
  for (PathedDep* d: _dynamic_deps) {
    d->bake_path();
  }
}



}

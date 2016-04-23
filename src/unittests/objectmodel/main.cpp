#include <base/memoryallocator/taggednew.h>
#include <base/memoryallocator/bootstrap.h>

#include <components/entities/nonguientities.h>
#include <unittests/objectmodel/testdeps.h>

#include <iostream>

int main(int argc, char **argv) {
  using namespace ngs;

  // Startup the memory tracker.
  bootstrap_memory_tracker();

  // Tests.
  {
    Entity* app_root = new_ff BaseAppEntity(NULL, "app");
    app_root->create_internals();

    Entity* root_group = new_ff BaseGroupNodeEntity(app_root, "root");
    root_group->create_internals();

    TestDeps* test_deps = new_ff TestDeps(app_root);
    app_root->initialize_deps();
    app_root->update_deps_and_hierarchy();

    test_deps->test();

    // Cleanup.
    delete_ff(app_root);
  }

  // Cleanup.
  shutdown_memory_tracker();
  return 0;
}

#include <base/memoryallocator/taggednew.h>
#include <base/memoryallocator/bootstrap.h>

#include <unittests/nodegraph/testplugs.h>
#include <unittests/nodegraph/testdatanodes.h>
#include <iostream>

int main(int argc, char **argv) {
  using namespace ngs;

  // Startup the memory tracker.
  bootstrap_memory_tracker();

  // Tests.
  {
//    TestPlugs test_plug;
//    TestDataNodes test_data_nodes;
  }

  // Cleanup.
  shutdown_memory_tracker();
  return 0;
}




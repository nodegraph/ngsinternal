#pragma once
#include <base/memoryallocator/memoryallocator_export.h>

namespace ngs {

void MEMORYALLOCATOR_EXPORT bootstrap_memory_tracker();
void MEMORYALLOCATOR_EXPORT shutdown_memory_tracker();

}

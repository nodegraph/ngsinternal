#include <base/utils/idgenerator.h>
#include <base/utils/permit.h>

#include <cassert>
#include <iostream>

namespace ngs{

IDGenerator::IDGenerator():
  _num_ids(0){}

IDGenerator::~IDGenerator(){}

void IDGenerator::check_permit(Permit<IDGenerator>& holder) {
  // Make sure the holder has locked us.
  if (!holder.is_for(this)) {
    assert(false);
  }
}

unsigned int IDGenerator::create_id(Permit<IDGenerator>& holder) {
  check_permit(holder);

  unsigned int id = -1;
  if (_destroyed.empty()) {
    assert(_num_ids != -1);
    id = _num_ids;
    ++_num_ids;
  } else {
    // Otherwise we reuse the first destroyed id.
    id = _destroyed.back();
    _destroyed.pop_back();
  }

  std::cerr << "created id: " << id << "\n";
  return id;
}

void IDGenerator::destroy_id(Permit<IDGenerator>& holder,unsigned int id) {
  check_permit(holder);

  assert(id<_num_ids);
  _destroyed.push_back(id);
}

}

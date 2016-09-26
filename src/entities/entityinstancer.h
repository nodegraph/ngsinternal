#pragma once
#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/entity.h>
#include <entities/entities_export.h>
#include <entities/guientities.h>
#include <entities/nonguientities.h>


namespace ngs {

class ENTITIES_EXPORT EntityInstancer: public BaseEntityInstancer {
 public:
  EntityInstancer() {}
  virtual ~EntityInstancer() {}
  virtual Entity* instance(Entity* parent, const std::string& name, size_t derived_id) const;
  virtual size_t get_iid(size_t derived_id) const;
  virtual const char* get_name_for_did(size_t derived_id) const;
};

}

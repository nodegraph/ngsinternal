#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/memoryallocator/taggednew.h>

#include <memory>
#include <unordered_set>
#include <unordered_map>

#include <cstddef>
#include <cassert>
#include <type_traits>

namespace ngs {

class Component;

class OBJECTMODEL_EXPORT DepLink {
 public:
  DepLink(Component* dependant, Component* dependency);
  ~DepLink();
  Component* dependant; // Becomes null when the dependant has been destroyed.
  Component* dependency; // Becomes null when the link is broken, but the DepLink shared_ptr is still outstanding.
  Component* const dependency_hash; // This is fixed and used for hashing in stl containers.
  size_t const iid; // This is fixed and used to remove the dep link, after the dependency is destroyed and there are no outstanding shared_ptrs.
};

typedef std::shared_ptr<DepLink> DepLinkPtr;
typedef std::weak_ptr<DepLink> DepLinkWPtr;

class OBJECTMODEL_EXPORT DepLinkHolder {
 public:
  virtual ~DepLinkHolder();
  virtual const DepLinkPtr& get_link() const = 0;
  virtual Component* get_component() const;
 protected:

};

}

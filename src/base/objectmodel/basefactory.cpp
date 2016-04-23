#include <base/objectmodel/objectmodel_export.h>
#include <base/objectmodel/basefactory.h>

namespace ngs {

BaseEntityInstancer::~BaseEntityInstancer() {}

BaseComponentInstancer::~BaseComponentInstancer() {}

BaseFactory::BaseFactory(Entity* entity, size_t did)
    : Component(entity, kIID(), did) {
}

}

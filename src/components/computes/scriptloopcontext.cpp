#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <components/computes/scriptloopcontext.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>



namespace ngs {

ScriptLoopContext::ScriptLoopContext(Entity* parent)
    : Component(parent, kIID(), kDID()) {
}

ScriptLoopContext::~ScriptLoopContext() {
}

}


#include <components/compshapes/dotnodeshape.h>
#include <components/compshapes/inputnodeshape.h>
#include <components/compshapes/rectnodeshape.h>
#include <components/compshapes/outputnodeshape.h>
#include <components/compshapes/compshapecollective.h>
#include <components/interactions/shapecanvas.h>
#include <components/interactions/groupinteraction.h>
#include <components/resources/resources.h>

#include <base/utils/simpleloader.h>
#include <base/utils/simplesaver.h>

#include <components/compshapes/inputshape.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/groupnodeshape.h>
#include <components/compshapes/inputlabelshape.h>
#include <components/compshapes/nodeselection.h>
#include <components/compshapes/outputlabelshape.h>
#include <components/compshapes/topology.h>

#include <components/computes/compute.h>
#include <components/computes/groupnodecompute.h>
#include <components/computes/ifgroupnodecompute.h>
#include <components/computes/whilegroupnodecompute.h>
#include <components/computes/mergenodecompute.h>
#include <components/computes/datanodecompute.h>
#include <components/computes/copydatanodecompute.h>
#include <components/computes/erasedatanodecompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/mocknodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/dotnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/inputs.h>
#include <components/computes/outputs.h>
#include <components/interactions/viewcontrols.h>
#include <components/interactions/graphbuilder.h>

#include <entities/factory.h>
#include <base/objectmodel/componentids.h>
#include <entities/componentinstancer.h>

#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/httpcomputes.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <guicomponents/computes/mqttcomputes.h>
#include <guicomponents/computes/postnodecompute.h>

#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/nodegraphcontroller.h>
#include <guicomponents/quick/nodegraphview.h>
#include <guicomponents/quick/nodegraphmanipulator.h>

#include <guicomponents/computes/messagesender.h>
#include <guicomponents/computes/messagereceiver.h>
#include <base/objectmodel/appconfig.h>
#include <components/computes/scriptloopcontext.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/computes/httpworker.h>
#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/comms/cryptologic.h>

#include <gui/widget/nodegrapheditor.h>
#include <guicomponents/computes/acceptsaveprocess.h>
#include <guicomponents/computes/downloadvideoprocess.h>
#include <guicomponents/computes/downloadmanager.h>
#include <guicomponents/computes/javaprocess.h>
#include <guicomponents/computes/browserrecorder.h>
#include <guicomponents/computes/mqtthostcompute.h>
#include <guicomponents/computes/taskqueuer.h>
#include <guicomponents/computes/waitnodecompute.h>
#include <iostream>
#include <cassert>

namespace ngs {


Component* ComponentInstancer::instance_imp(Entity* entity, ComponentDID did) {
  // Define our component type traits.
  #undef COMPONENT_ENTRY1
  #undef COMPONENT_ENTRY2
  #define COMPONENT_ENTRY1(NAME) case ComponentDID::k##NAME: return new_ff NAME(entity);
  #define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  switch (did) {
    COMPONENT_ENTRIES()
  }
  std::cerr << "Error: ComponentInstancer is unable to create entity with derived id: " << (int)did << "\n";
  assert(false);
  return NULL;
}

Component* ComponentInstancer::instance(Entity* entity, ComponentDID did) const{
  return instance_imp(entity, did);
}

ComponentIID ComponentInstancer::get_iid_for_did(ComponentDID did) const {
  #undef COMPONENT_ENTRY1
  #undef COMPONENT_ENTRY2
  #define COMPONENT_ENTRY1(NAME) case ComponentDID::k##NAME: return NAME::kIID();
  #define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  switch(did) {
    COMPONENT_ENTRIES()
  }
  std::cerr << "Error: ComponentInstancer is not supposed to see this derived id in a file: " << (int)did << "\n";
  assert(false);
  return ComponentIID::kIInvalidComponent;
}


}



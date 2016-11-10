
#include <components/compshapes/dotnodeshape.h>
#include <components/compshapes/inputnodeshape.h>
#include <components/compshapes/rectnodeshape.h>
#include <components/compshapes/outputnodeshape.h>
#include <components/compshapes/compshapecollective.h>
#include <components/interactions/canvas.h>
#include <components/interactions/shapecanvas.h>
#include <components/interactions/groupinteraction.h>
#include <guicomponents/comms/grouptraits.h>
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
#include <components/computes/mergenodecompute.h>
#include <components/computes/datanodecompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/mocknodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/dotnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/inputs.h>
#include <components/computes/outputs.h>

#include <entities/componentinstancer.h>
#include <components/interactions/viewcontrols.h>
#include <components/interactions/graphbuilder.h>

#include <entities/factory.h>
#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/httpcomputes.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <guicomponents/computes/firebasegroupnodecompute.h>
#include <guicomponents/computes/mqttgroupnodecompute.h>
#include <guicomponents/computes/mqttcomputes.h>

#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/nodegraphrenderer.h>
#include <guicomponents/quick/nodegraphview.h>
#include <guicomponents/quick/nodegraphmanipulator.h>

#include <guicomponents/comms/nodejsprocess.h>
#include <guicomponents/comms/messagesender.h>
#include <guicomponents/comms/messagereceiver.h>
#include <guicomponents/comms/appconfig.h>
#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/httpworker.h>
#include <guicomponents/computes/mqttworker.h>
#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/comms/cryptologic.h>
#include <guicomponents/comms/firebasegrouptraits.h>
#include <guicomponents/computes/mqttgrouptraits.h>

#include <gui/widget/nodegrapheditor.h>
#include <guicomponents/comms/browsergrouptraits.h>
#include <guicomponents/comms/browserrecorder.h>
#include <guicomponents/comms/browserworker.h>
#include <guicomponents/computes/browsergroupnodecompute.h>
#include <guicomponents/computes/scriptgroupnodecompute.h>
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

const char* ComponentInstancer::get_compute_name(ComponentDID compute_did) const {
  if (compute_did == ComponentDID::kDotNodeCompute) {
    return "dot";
  } else if (compute_did == ComponentDID::kInputCompute) {
    return "input internal";
  } else if (compute_did == ComponentDID::kOutputCompute) {
    return "output internal";
  } else if (compute_did == ComponentDID::kGroupNodeCompute) {
    return "group";
  } else if (compute_did == ComponentDID::kInputNodeCompute) {
    return "input";
  } else if (compute_did == ComponentDID::kOutputNodeCompute) {
    return "output";
  } else if (compute_did == ComponentDID::kScriptNodeCompute) {
    return "script";
  } else if (compute_did == ComponentDID::kMockNodeCompute) {
    return "mock";
  } else if (compute_did == ComponentDID::kOpenBrowserCompute) {
    return "open browser";
  } else if (compute_did == ComponentDID::kCloseBrowserCompute) {
    return "close browser";
  } else if (compute_did == ComponentDID::kIsBrowserOpenCompute) {
    return "is browser open";
  } else if (compute_did == ComponentDID::kResizeBrowserCompute) {
    return "resize browser";
  } else if (compute_did == ComponentDID::kNavigateToCompute) {
    return "navigate to";
  } else if (compute_did == ComponentDID::kNavigateRefreshCompute) {
    return "navigate refresh";
  } else if (compute_did == ComponentDID::kSwitchToIFrameCompute) {
    return "switch to iframe";
  } else if (compute_did == ComponentDID::kCreateSetFromValuesCompute) {
    return "create set from values";
  } else if (compute_did == ComponentDID::kCreateSetFromTypeCompute) {
    return "create set from types";
  } else if (compute_did == ComponentDID::kDeleteSetCompute) {
    return "delete set";
  } else if (compute_did == ComponentDID::kShiftSetCompute) {
    return "shift set";
  } else if (compute_did == ComponentDID::kMouseActionCompute) {
    return "mouse action";
  } else if (compute_did == ComponentDID::kStartMouseHoverActionCompute) {
    return "start hover";
  } else if (compute_did == ComponentDID::kStopMouseHoverActionCompute) {
    return "stop hover";
  } else if (compute_did == ComponentDID::kTextActionCompute) {
    return "text action";
  } else if (compute_did == ComponentDID::kElementActionCompute) {
    return "element action";
  } else if (compute_did == ComponentDID::kExpandSetCompute) {
    return "expand set";
  } else if (compute_did == ComponentDID::kMarkSetCompute) {
    return "mark set";
  } else if (compute_did == ComponentDID::kUnmarkSetCompute) {
    return "unmark set";
  } else if (compute_did == ComponentDID::kMergeSetsCompute) {
    return "merge sets";
  } else if (compute_did == ComponentDID::kShrinkSetToSideCompute) {
    return "shrink set to side";
  } else if (compute_did == ComponentDID::kShrinkAgainstMarkedCompute) {
    return "shrink set against marked";
  }
  return "unknown node";
}

}



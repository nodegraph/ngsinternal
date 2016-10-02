
#include <components/compshapes/dotnodeshape.h>
#include <components/compshapes/inputnodeshape.h>
#include <components/compshapes/rectnodeshape.h>
#include <components/compshapes/outputnodeshape.h>
#include <components/compshapes/compshapecollective.h>
#include <components/interactions/canvas.h>
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

#include <components/computes/compute.h>
#include <components/computes/groupnodecompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/mocknodecompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/dotnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <entities/componentinstancer.h>
#include <components/interactions/viewcontrols.h>
#include <components/interactions/graphbuilder.h>

#include <entities/factory.h>
#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>
#include <guicomponents/quick/nodegraphrenderer.h>
#include <guicomponents/quick/nodegraphview.h>
#include <guicomponents/comms/appcomm.h>
#include <guicomponents/comms/appworker.h>
#include <guicomponents/comms/licensechecker.h>
#include <gui/widget/nodegrapheditor.h>


#include <iostream>
#include <cassert>

namespace ngs {

// An invalid component which should never be created.
class InvalidComponent: public Component {
 public:
  InvalidComponent(Entity* entity):Component(entity, kIID(), kDID()) {}
  COMPONENT_ID(InvalidComponent, InvalidComponent);
};


#define COMPONENT_CASES(case_statement)\
    /* Factories. */\
    case_statement(Factory)\
    /* Comp Shapes. */\
    case_statement(DotNodeShape)\
    case_statement(GroupNodeShape)\
    case_statement(InputNodeShape)\
    case_statement(InputLabelShape)\
    case_statement(InputShape)\
    case_statement(LinkShape)\
    case_statement(RectNodeShape)\
    case_statement(OutputNodeShape)\
    case_statement(OutputLabelShape)\
    case_statement(OutputShape)\
    /* Comp Shape Collective. */\
    case_statement(CompShapeCollective)\
    /* Computes. */\
    case_statement(DotNodeCompute)\
    case_statement(InputCompute)\
    case_statement(OutputCompute)\
    case_statement(GroupNodeCompute)\
    case_statement(InputNodeCompute)\
    case_statement(ScriptNodeCompute)\
    case_statement(MockNodeCompute)\
    case_statement(OutputNodeCompute)\
    case_statement(OpenBrowserCompute)\
    case_statement(CloseBrowserCompute)\
    case_statement(CreateSetFromValuesCompute)\
    case_statement(CreateSetFromTypeCompute)\
    /* Graph Building. */\
    case_statement(GraphBuilder)\
    /* Interactions. */\
    case_statement(GroupInteraction)\
    /* App Level. */\
    case_statement(NodeSelection)\
    case_statement(Resources)\
    case_statement(ShapeCanvas)\
    /* Invalid and Components which must be manually created. */\
    case ComponentDID::kInvalidComponent:\
    case ComponentDID::kTestComponent:\
    case ComponentDID::kNodeGraphEditor:\
    case ComponentDID::kNodeGraphQuickItem:\
    case ComponentDID::kFBORenderer:\
    case ComponentDID::kFBOWorker:\
      break;

#define INST_CASE(C) case ComponentDID::k##C: return new_ff C(entity);

Component* ComponentInstancer::instance(Entity* entity, ComponentDID did) const{
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

#define IID_CASE(C) case ComponentDID::k##C: return C::kIID();

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



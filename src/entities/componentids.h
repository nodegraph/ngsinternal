#pragma once
#include <entities/entities_export.h>

namespace ngs {

// These macros should be used in class declarations.
// These IDs are used heavily in serialization and identifying
// components in Entities.

// We allows derived Components to form interfaces,
// or to implement other interfaces.
#define COMPONENT_ID(base,derived)\
  static ComponentIID kIID()\
  { return ComponentIID::kI##base; }\
  \
  static ComponentDID kDID()\
  { return ComponentDID::k##derived; }

// Component Interface IDs.
enum class ComponentIID : size_t {
  kIInvalidComponent,
  kIBaseFactory,
  kIInputTopology,
  kIOutputTopology,
  kICompShape,
  kICompShapeCollective,
  kICompute,
  kIInputs,
  kIOutputs,
  kIFBORenderer,
  kIFBOWorker,
  kIGraphBuilder,
  kILowerHierarchyChange,
  kIUpperHierarchyChange,
  kIInputPlug,
  kIGroupInteraction,
  kINodeGraphEditor,
  kIBaseNodeGraphManipulator,
  kINodeGraphQuickItem,
  kINodeGraphRenderer,
  kINodeGraphView,
  kIFileModel,
  kINodeJSProcess,
  kIMessageSender,
  kIMessageReceiver,
  kIAppConfig,
  //kIAppComm,
  kIWebWorker,
  kIWebRecorder,
  kITaskScheduler,
  kILicenseChecker,
  kILinkGroup,
  kINodeSelection,
  kIOutputPlug,
  kIResources,
  kIShapeCanvas,
  kITestComponent,

};

#define COMPONENT_ENTRY1(NAME) k##NAME,
#define COMPONENT_ENTRY2(NAME, VALUE) k##NAME = VALUE,

#define COMPONENT_ENTRIES()\
COMPONENT_ENTRY1(InvalidComponent)\
/* Factories */\
COMPONENT_ENTRY1(Factory)\
/* CompShapes. */\
COMPONENT_ENTRY1(InputTopology)\
COMPONENT_ENTRY1(OutputTopology)\
COMPONENT_ENTRY1(DotNodeShape)\
COMPONENT_ENTRY1(GroupNodeShape)\
COMPONENT_ENTRY1(InputNodeShape)\
COMPONENT_ENTRY1(InputLabelShape)\
COMPONENT_ENTRY1(InputShape)\
COMPONENT_ENTRY1(LinkShape)\
COMPONENT_ENTRY1(RectNodeShape)\
COMPONENT_ENTRY1(OutputNodeShape)\
COMPONENT_ENTRY1(OutputLabelShape)\
COMPONENT_ENTRY1(OutputShape)\
/* -- */\
COMPONENT_ENTRY1(CompShapeCollective)\
/* Computes. */\
COMPONENT_ENTRY1(Inputs)\
COMPONENT_ENTRY1(Outputs)\
COMPONENT_ENTRY1(DotNodeCompute)\
COMPONENT_ENTRY1(InputCompute)\
COMPONENT_ENTRY1(OutputCompute)\
COMPONENT_ENTRY1(GroupNodeCompute)\
COMPONENT_ENTRY1(InputNodeCompute)\
COMPONENT_ENTRY1(OutputNodeCompute)\
COMPONENT_ENTRY1(ScriptNodeCompute)\
COMPONENT_ENTRY1(MockNodeCompute)\
COMPONENT_ENTRY1(OpenBrowserCompute)\
COMPONENT_ENTRY1(CloseBrowserCompute)\
COMPONENT_ENTRY1(IsBrowserOpenCompute)\
COMPONENT_ENTRY1(ResizeBrowserCompute)\
COMPONENT_ENTRY1(NavigateToCompute)\
COMPONENT_ENTRY1(NavigateRefreshCompute)\
COMPONENT_ENTRY1(SwitchToIFrameCompute)\
COMPONENT_ENTRY1(CreateSetFromValuesCompute)\
COMPONENT_ENTRY1(CreateSetFromTypeCompute)\
COMPONENT_ENTRY1(DeleteSetCompute)\
COMPONENT_ENTRY1(ShiftSetCompute)\
COMPONENT_ENTRY1(MouseActionCompute)\
COMPONENT_ENTRY1(StartMouseHoverActionCompute)\
COMPONENT_ENTRY1(StopMouseHoverActionCompute)\
COMPONENT_ENTRY1(TextActionCompute)\
COMPONENT_ENTRY1(ElementActionCompute)\
COMPONENT_ENTRY1(ExpandSetCompute)\
COMPONENT_ENTRY1(MarkSetCompute)\
COMPONENT_ENTRY1(UnmarkSetCompute)\
COMPONENT_ENTRY1(MergeSetsCompute)\
COMPONENT_ENTRY1(ShrinkSetToSideCompute)\
COMPONENT_ENTRY1(ShrinkAgainstMarkedCompute)\
/* -- */\
COMPONENT_ENTRY1(FBORenderer)\
COMPONENT_ENTRY1(FBOWorker)\
COMPONENT_ENTRY1(GraphBuilder)\
/* Interactions. */\
COMPONENT_ENTRY1(GroupInteraction)\
/* -- */\
COMPONENT_ENTRY1(NodeGraphEditor)\
COMPONENT_ENTRY1(NodeGraphManipulator)\
COMPONENT_ENTRY1(NodeGraphQuickItem)\
COMPONENT_ENTRY1(NodeGraphRenderer)\
COMPONENT_ENTRY1(NodeGraphView)\
COMPONENT_ENTRY1(FileModel)\
COMPONENT_ENTRY1(NodeJSProcess)\
COMPONENT_ENTRY1(MessageSender)\
COMPONENT_ENTRY1(MessageReceiver)\
COMPONENT_ENTRY1(AppConfig)\
/*COMPONENT_ENTRY1(AppComm)\*/\
COMPONENT_ENTRY1(WebWorker)\
COMPONENT_ENTRY1(WebRecorder)\
COMPONENT_ENTRY1(TaskScheduler)\
COMPONENT_ENTRY1(LicenseChecker)\
/* -- */\
COMPONENT_ENTRY1(NodeSelection)\
COMPONENT_ENTRY1(Resources)\
COMPONENT_ENTRY1(ShapeCanvas)\












enum class ComponentDID : size_t {
  COMPONENT_ENTRIES()
};


ENTITIES_EXPORT const char* get_component_did_name(ComponentDID did);

}

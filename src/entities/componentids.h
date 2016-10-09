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
  kICompShape,
  kICompShapeCollective,
  kICompute,
  kIFBORenderer,
  kIFBOWorker,
  kIGraphBuilder,
  kILowerHierarchyChange,
  kIUpperHierarchyChange,
  kIInputPlug,
  kIGroupInteraction,
  kINodeGraphEditor,
  kIBaseVisualizeProcessing,
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
  kIBaseInputs,
  kIBaseOutputs,
};

#define COMPONENT_ENTRY1(NAME) k##NAME,
#define COMPONENT_ENTRY2(NAME, VALUE) k##NAME = VALUE,

#define COMPONENT_ENTRIES()\
COMPONENT_ENTRY2(InvalidComponent, 0)\
/* Factories */\
COMPONENT_ENTRY2(Factory, 1)\
/* CompShapes. */\
COMPONENT_ENTRY2(DotNodeShape, 2)\
COMPONENT_ENTRY2(GroupNodeShape, 3)\
COMPONENT_ENTRY2(InputNodeShape, 4)\
COMPONENT_ENTRY2(InputLabelShape, 5)\
COMPONENT_ENTRY2(InputShape, 6)\
COMPONENT_ENTRY2(LinkShape, 7)\
COMPONENT_ENTRY2(RectNodeShape, 8)\
COMPONENT_ENTRY2(OutputNodeShape, 9)\
COMPONENT_ENTRY2(OutputLabelShape, 10)\
COMPONENT_ENTRY2(OutputShape, 11)\
/* -- */\
COMPONENT_ENTRY2(CompShapeCollective, 12)\
/* Computes. */\
COMPONENT_ENTRY2(DotNodeCompute, 13)\
COMPONENT_ENTRY2(InputCompute, 14)\
COMPONENT_ENTRY2(OutputCompute, 15)\
COMPONENT_ENTRY2(GroupNodeCompute, 16)\
COMPONENT_ENTRY2(InputNodeCompute, 17)\
COMPONENT_ENTRY2(OutputNodeCompute, 18)\
COMPONENT_ENTRY2(ScriptNodeCompute, 19)\
COMPONENT_ENTRY2(MockNodeCompute, 20)\
COMPONENT_ENTRY2(OpenBrowserCompute, 21)\
COMPONENT_ENTRY2(CloseBrowserCompute, 22)\
COMPONENT_ENTRY2(IsBrowserOpenCompute, 23)\
COMPONENT_ENTRY2(ResizeBrowserCompute, 24)\
COMPONENT_ENTRY2(NavigateToCompute, 25)\
COMPONENT_ENTRY2(NavigateRefreshCompute, 26)\
COMPONENT_ENTRY2(SwitchToIFrameCompute, 27)\
COMPONENT_ENTRY2(CreateSetFromValuesCompute, 28)\
COMPONENT_ENTRY2(CreateSetFromTypeCompute, 29)\
COMPONENT_ENTRY2(DeleteSetCompute, 30)\
COMPONENT_ENTRY2(ShiftSetCompute, 31)\
COMPONENT_ENTRY2(MouseActionCompute, 32)\
COMPONENT_ENTRY2(StartMouseHoverActionCompute, 33)\
COMPONENT_ENTRY2(StopMouseHoverActionCompute, 34)\
COMPONENT_ENTRY2(TextActionCompute, 35)\
COMPONENT_ENTRY2(ElementActionCompute, 36)\
COMPONENT_ENTRY2(ExpandSetCompute, 37)\
COMPONENT_ENTRY2(MarkSetCompute, 38)\
COMPONENT_ENTRY2(UnmarkSetCompute, 39)\
COMPONENT_ENTRY2(MergeSetsCompute, 40)\
COMPONENT_ENTRY2(ShrinkSetToSideCompute, 41)\
COMPONENT_ENTRY2(ShrinkAgainstMarkedCompute, 42)\
/* -- */\
COMPONENT_ENTRY2(FBORenderer, 43)\
COMPONENT_ENTRY2(FBOWorker, 44)\
COMPONENT_ENTRY2(GraphBuilder, 45)\
/* Interactions. */\
COMPONENT_ENTRY2(GroupInteraction, 46)\
/* -- */\
COMPONENT_ENTRY2(NodeGraphEditor, 47)\
COMPONENT_ENTRY2(NodeGraphQuickItem, 48)\
COMPONENT_ENTRY2(NodeGraphRenderer, 49)\
COMPONENT_ENTRY2(NodeGraphView, 50)\
COMPONENT_ENTRY2(FileModel, 51)\
/*COMPONENT_ENTRY2(AppComm, 52)\*/\
COMPONENT_ENTRY2(WebWorker, 53)\
COMPONENT_ENTRY2(LicenseChecker, 54)\
/* -- */\
COMPONENT_ENTRY2(NodeSelection, 55)\
COMPONENT_ENTRY2(Resources, 56)\
COMPONENT_ENTRY2(ShapeCanvas, 57)\
/* -- */\
COMPONENT_ENTRY2(Inputs, 58)\
COMPONENT_ENTRY2(Outputs, 59)\
COMPONENT_ENTRY2(WebRecorder, 60)\
COMPONENT_ENTRY2(TaskScheduler, 61)\
COMPONENT_ENTRY2(NodeJSProcess, 62)\
COMPONENT_ENTRY2(AppConfig, 63)\
COMPONENT_ENTRY2(MessageSender, 64)\
COMPONENT_ENTRY2(MessageReceiver, 65)\
COMPONENT_ENTRY2(VisualizeProcessing, 66)

enum class ComponentDID : size_t {
  COMPONENT_ENTRIES()
};


ENTITIES_EXPORT const char* get_component_did_name(ComponentDID did);

}

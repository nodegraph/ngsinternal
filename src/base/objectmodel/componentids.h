#pragma once
#include <base/objectmodel/objectmodel_export.h>
#include <base/utils/enumutil.h>
#include <string>

namespace ngs {

// These macros should be used in class declarations.
// These IDs are used heavily in serialization and identifying
// components in Entities.

// We allows derived Components to form interfaces,
// or to implement other interfaces.
#define COMPONENT_ID(base, derived)\
  static ComponentIID kIID()\
  { return ComponentIID::kI##base; }\
  virtual ComponentIID get_iid() const\
  { return kIID(); }\
  \
  static ComponentDID kDID()\
  { return ComponentDID::k##derived; }\
  virtual ComponentDID get_did() const\
  { return kDID(); }\
  \
  static const char* kIIDName()\
  { return #base; }\
  virtual const char* get_iid_name() const\
  {return kIIDName();}\
  \
  static const char* kDIDName()\
  { return #derived; }\
  virtual const char* get_did_name() const\
  {return kDIDName(); }\


// Component Interface IDs.
enum class ComponentIID : size_t {
  kIInvalidComponent = 0,

  // Factory.
  kIBaseFactory = 1,

  // ---------------------------------------------
  // App.
  // ---------------------------------------------

  // App Logic.
  kIResources = 2,
  kIAppConfig = 3,
  kILicenseChecker = 4,
  kICryptoLogic = 5,
  kIFileModel = 6,

  // ---------------------------------------------
  // GUI.
  // ---------------------------------------------

  // Widgets.
  kINodeGraphEditor = 7,

  // Quick Items.
  kINodeGraphQuickItem = 8,
  kINodeGraphView = 9,

  // ---------------------------------------------
  // Rendering.
  // ---------------------------------------------

  // Rendering.
  kIFBORenderer = 10,
  kIFBOWorker = 11,

  // Shapes.
  kIShapeCanvas = 12,
  kICompShapeCollective = 13,
  kICompShape = 14,

  // ---------------------------------------------
  // Node.
  // ---------------------------------------------

  // Node Topology.
  kIInputTopology = 15,
  kIOutputTopology = 16,
  kIInputs = 17,
  kIOutputs = 18,

  // ---------------------------------------------
  // Group Node.
  // ---------------------------------------------
  // Script context. (Used to hold state between iteractions in looping groups.)
  kIScriptLoopContext = 19,

  // ---------------------------------------------
  // Node Graph Interaction.
  // ---------------------------------------------

  // Node Graph Logic.
  kIGraphBuilder = 20,
  kIGroupInteraction = 21,
  kINodeSelection = 22,
  kIBaseNodeGraphManipulator = 23,
  kINodeGraphController = 24,

  // ---------------------------------------------
  // Node Graph Compute Management.
  // ---------------------------------------------

  // App Communication.
  kIMessageSender = 25,
  kIMessageReceiver = 26,

  // Special Processes.
  kIAcceptSaveProcess = 27,
  kIJavaProcess = 28,
  kIDownloadManager = 29,

  // Task Management.
  kITaskQueuer = 30,
  kITaskScheduler = 31,

  // Task Management Helpers.
  kIHTTPWorker = 32,
  kIBrowserRecorder = 33,

  // Node Computes.
  kICompute = 34,



};

#define COMPONENT_ENTRY1(NAME) k##NAME,
#define COMPONENT_ENTRY2(NAME, VALUE) k##NAME = VALUE,

#define COMPONENT_ENTRIES()\
COMPONENT_ENTRY2(InvalidComponent, 0)\
/* Factories */\
COMPONENT_ENTRY2(Factory, 1)\
/* App. ------------------------------*/\
/* App Logic. */\
COMPONENT_ENTRY2(Resources, 2)\
COMPONENT_ENTRY2(AppConfig, 3)\
COMPONENT_ENTRY2(LicenseChecker, 4)\
COMPONENT_ENTRY2(CryptoLogic, 5)\
COMPONENT_ENTRY2(FileModel, 6)\
/* Gui. -------------------------------*/\
/* Widgets. */\
COMPONENT_ENTRY2(NodeGraphEditor, 7)\
/* Quick Items. */\
COMPONENT_ENTRY2(NodeGraphQuickItem, 8)\
COMPONENT_ENTRY2(NodeGraphView, 9)\
/* Rendering. -------------------------*/\
/* Rendering.*/\
COMPONENT_ENTRY2(FBORenderer, 10)\
COMPONENT_ENTRY2(FBOWorker, 11)\
/* Shapes. */\
COMPONENT_ENTRY2(ShapeCanvas, 12)\
COMPONENT_ENTRY2(CompShapeCollective, 13)\
COMPONENT_ENTRY2(InputShape, 14)\
COMPONENT_ENTRY2(OutputShape, 15)\
COMPONENT_ENTRY2(InputLabelShape, 16)\
COMPONENT_ENTRY2(OutputLabelShape, 17)\
COMPONENT_ENTRY2(LinkShape, 18)\
COMPONENT_ENTRY2(RectNodeShape, 19)\
COMPONENT_ENTRY2(GroupNodeShape, 20)\
COMPONENT_ENTRY2(DotNodeShape, 21)\
COMPONENT_ENTRY2(InputNodeShape, 22)\
COMPONENT_ENTRY2(OutputNodeShape, 23)\
/* Node ------------------------------*/\
/* Node Topology. */\
COMPONENT_ENTRY2(InputTopology, 24)\
COMPONENT_ENTRY2(OutputTopology, 25)\
COMPONENT_ENTRY2(Inputs, 26)\
COMPONENT_ENTRY2(Outputs, 27)\
/* Group Node. */\
COMPONENT_ENTRY2(ScriptLoopContext, 28)\
/* Node Graph Interaction. -----------*/\
/* Node Graph Logic. */\
COMPONENT_ENTRY2(GraphBuilder, 29)\
COMPONENT_ENTRY2(GroupInteraction, 30)\
COMPONENT_ENTRY2(NodeSelection, 31)\
COMPONENT_ENTRY2(NodeGraphManipulator, 32)\
COMPONENT_ENTRY2(NodeGraphController, 33)\
/* Node Graph Compute Management.*/\
/* App Communication. */\
COMPONENT_ENTRY2(MessageSender, 34)\
COMPONENT_ENTRY2(MessageReceiver, 35)\
/* Special Processes. */\
COMPONENT_ENTRY2(AcceptSaveProcess, 36)\
COMPONENT_ENTRY2(JavaProcess, 37)\
COMPONENT_ENTRY2(DownloadManager, 38)\
/* Task Management. */\
COMPONENT_ENTRY2(TaskQueuer, 39)\
COMPONENT_ENTRY2(TaskScheduler,40)\
/* Task Management Helpers.*/\
COMPONENT_ENTRY2(HTTPWorker, 41)\
COMPONENT_ENTRY2(BrowserRecorder, 42)\
/* Node Computes. */\
COMPONENT_ENTRY2(InputCompute, 43)\
COMPONENT_ENTRY2(OutputCompute, 44)\
COMPONENT_ENTRY2(InputNodeCompute, 45)\
COMPONENT_ENTRY2(OutputNodeCompute, 46)\
COMPONENT_ENTRY2(DotNodeCompute, 47)\
/* Group Computes. */\
COMPONENT_ENTRY2(GroupNodeCompute, 48)\
COMPONENT_ENTRY2(IfGroupNodeCompute, 49)\
COMPONENT_ENTRY2(WhileGroupNodeCompute, 50)\
/* Protocol Based Computes. */\
COMPONENT_ENTRY2(MQTTHostCompute, 51)\
COMPONENT_ENTRY2(HTTPCompute, 56)\
COMPONENT_ENTRY2(MQTTPublishCompute, 57)\
COMPONENT_ENTRY2(MQTTSubscribeCompute, 58)\
/* Data Op Computes. */\
COMPONENT_ENTRY2(MockNodeCompute, 59)\
COMPONENT_ENTRY2(DataNodeCompute, 60)\
COMPONENT_ENTRY2(CopyDataNodeCompute, 61)\
COMPONENT_ENTRY2(EraseDataNodeCompute, 62)\
COMPONENT_ENTRY2(MergeNodeCompute, 63)\
COMPONENT_ENTRY2(ScriptNodeCompute, 64)\
COMPONENT_ENTRY2(PostNodeCompute, 65)\
COMPONENT_ENTRY2(WaitNodeCompute, 66)\
/* Perform action on Browser. */\
COMPONENT_ENTRY2(OpenBrowserCompute, 67)\
COMPONENT_ENTRY2(CloseBrowserCompute, 68)\
COMPONENT_ENTRY2(ReleaseBrowserCompute, 69)\
COMPONENT_ENTRY2(IsBrowserOpenCompute, 70)\
COMPONENT_ENTRY2(ResizeBrowserCompute, 71)\
COMPONENT_ENTRY2(GetBrowserSizeCompute, 72)\
COMPONENT_ENTRY2(DestroyCurrentTabCompute, 73)\
COMPONENT_ENTRY2(OpenTabCompute, 74)\
COMPONENT_ENTRY2(AcceptSaveDialogCompute, 75)\
/* Navigate. */\
COMPONENT_ENTRY2(NavigateToCompute, 76)\
COMPONENT_ENTRY2(NavigateBackCompute, 77)\
COMPONENT_ENTRY2(NavigateForwardCompute, 78)\
COMPONENT_ENTRY2(NavigateRefreshCompute, 79)\
/* Find Elements. */\
COMPONENT_ENTRY2(FindElementByPositionCompute, 80)\
COMPONENT_ENTRY2(FindElementByValuesCompute, 81)\
COMPONENT_ENTRY2(FindElementByTypeCompute, 82)\
/* Shift Element. */\
COMPONENT_ENTRY2(ShiftElementByTypeCompute, 83)\
COMPONENT_ENTRY2(ShiftElementByValuesCompute, 84)\
COMPONENT_ENTRY2(ShiftElementByTypeAlongRowsCompute, 85)\
COMPONENT_ENTRY2(ShiftElementByValuesAlongRowsCompute, 86)\
COMPONENT_ENTRY2(ShiftElementByTypeAlongColumnsCompute, 87)\
COMPONENT_ENTRY2(ShiftElementByValuesAlongColumnsCompute, 88)\
/* Find Element Clusters. */\
/* Set and clear the current element.*/\
COMPONENT_ENTRY2(SetElementCompute, 89)\
/* Extract Info. */\
COMPONENT_ENTRY2(GetActiveTabTitleCompute, 90)\
COMPONENT_ENTRY2(GetCurrentURLCompute, 91)\
COMPONENT_ENTRY2(DownloadVideoCompute, 92)\
/* Perform action on Element. */\
COMPONENT_ENTRY2(MouseActionCompute, 93)\
COMPONENT_ENTRY2(TextActionCompute, 94)\
COMPONENT_ENTRY2(PasswordActionCompute, 95)\
COMPONENT_ENTRY2(ElementActionCompute, 96)\
/* Scroll. */\
COMPONENT_ENTRY2(ElementScrollCompute, 97)\
/* Password based input nodes. */\
COMPONENT_ENTRY2(PasswordInputNodeCompute, 98)\
COMPONENT_ENTRY2(PasswordDataNodeCompute, 99)\

// Numbers that are currently unused:
// 52, 53, 54, 55

enum class ComponentDID : size_t {
  COMPONENT_ENTRIES()
};

// Returns the derived class name as a string.
OBJECTMODEL_EXPORT const char* get_component_did_name(ComponentDID did);

// Returns a more user friendly name for the derived class.
OBJECTMODEL_EXPORT std::string get_user_friendly_name(const std::string& name);
OBJECTMODEL_EXPORT std::string get_component_user_did_name(ComponentDID did);

}


namespace std {
  template <> struct hash<ngs::ComponentDID> {
    size_t operator()(const ngs::ComponentDID& x) const {
     return ngs::to_underlying(x);
    }
  };
  template <> struct hash<ngs::ComponentIID> {
    size_t operator()(const ngs::ComponentIID& x) const {
     return ngs::to_underlying(x);
    }
  };
}

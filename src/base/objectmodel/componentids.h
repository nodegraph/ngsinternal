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
  kINodeGraphController,
  kINodeGraphRenderer,
  kINodeGraphView,
  kIFileModel,
  kINodeJSProcess,
  kIAcceptSaveProcess,
  kIJavaProcess,
  kIMessageSender,
  kIMessageReceiver,
  kIAppConfig,
  //kIAppComm,
  kINodeJSWorker,
  kIHTTPWorker,
  kIBrowserRecorder,
  kITaskScheduler,
  kILicenseChecker,
  kICryptoLogic,
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
COMPONENT_ENTRY1(IfGroupNodeCompute)\
COMPONENT_ENTRY1(ForEachGroupNodeCompute)\
COMPONENT_ENTRY1(WhileGroupNodeCompute)\
COMPONENT_ENTRY1(ScriptGroupNodeCompute)\
COMPONENT_ENTRY1(DataNodeCompute)\
COMPONENT_ENTRY1(CopyDataNodeCompute)\
COMPONENT_ENTRY1(EraseDataNodeCompute)\
COMPONENT_ENTRY1(LoopDataNodeCompute)\
COMPONENT_ENTRY1(AccumulateDataNodeCompute)\
COMPONENT_ENTRY1(MergeNodeCompute)\
COMPONENT_ENTRY1(InputNodeCompute)\
COMPONENT_ENTRY1(OutputNodeCompute)\
COMPONENT_ENTRY1(ScriptNodeCompute)\
COMPONENT_ENTRY1(MockNodeCompute)\
\
COMPONENT_ENTRY1(WaitNodeCompute)\
COMPONENT_ENTRY1(OpenBrowserCompute)\
COMPONENT_ENTRY1(CloseBrowserCompute)\
COMPONENT_ENTRY1(IsBrowserOpenCompute)\
COMPONENT_ENTRY1(ResizeBrowserCompute)\
COMPONENT_ENTRY1(GetActiveTabTitleCompute)\
COMPONENT_ENTRY1(DestroyCurrentTabCompute)\
COMPONENT_ENTRY1(OpenTabCompute)\
COMPONENT_ENTRY1(DownloadFilesCompute)\
COMPONENT_ENTRY1(NavigateToCompute)\
COMPONENT_ENTRY1(NavigateBackCompute)\
COMPONENT_ENTRY1(NavigateForwardCompute)\
COMPONENT_ENTRY1(NavigateRefreshCompute)\
COMPONENT_ENTRY1(GetCurrentURLCompute)\
COMPONENT_ENTRY1(FindElementByPositionCompute)\
COMPONENT_ENTRY1(FindElementByValuesCompute)\
COMPONENT_ENTRY1(FindElementByTypeCompute)\
COMPONENT_ENTRY1(ShiftElementByTypeCompute)\
COMPONENT_ENTRY1(ShiftElementByValuesCompute)\
COMPONENT_ENTRY1(MouseActionCompute)\
COMPONENT_ENTRY1(TextActionCompute)\
COMPONENT_ENTRY1(ElementActionCompute)\
\
COMPONENT_ENTRY1(EnterGroupCompute)\
COMPONENT_ENTRY1(EnterMQTTGroupCompute)\
COMPONENT_ENTRY1(EnterFirebaseGroupCompute)\
COMPONENT_ENTRY1(EnterBrowserGroupCompute)\
\
COMPONENT_ENTRY1(ExitBrowserGroupCompute)\
\
COMPONENT_ENTRY1(ExitGroupCompute)\
\
COMPONENT_ENTRY1(FirebaseWriteDataCompute)\
COMPONENT_ENTRY1(FirebaseReadDataCompute)\
\
COMPONENT_ENTRY1(HTTPCompute)\
COMPONENT_ENTRY1(MQTTPublishCompute)\
COMPONENT_ENTRY1(MQTTSubscribeCompute)\
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
COMPONENT_ENTRY1(NodeGraphController)\
COMPONENT_ENTRY1(NodeGraphRenderer)\
COMPONENT_ENTRY1(NodeGraphView)\
COMPONENT_ENTRY1(FileModel)\
COMPONENT_ENTRY1(NodeJSProcess)\
COMPONENT_ENTRY1(AcceptSaveProcess)\
COMPONENT_ENTRY1(JavaProcess)\
COMPONENT_ENTRY1(MessageSender)\
COMPONENT_ENTRY1(MessageReceiver)\
COMPONENT_ENTRY1(AppConfig)\
/*COMPONENT_ENTRY1(AppComm)\*/\
COMPONENT_ENTRY1(NodeJSWorker)\
COMPONENT_ENTRY1(HTTPWorker)\
COMPONENT_ENTRY1(BrowserRecorder)\
COMPONENT_ENTRY1(TaskScheduler)\
COMPONENT_ENTRY1(LicenseChecker)\
COMPONENT_ENTRY1(CryptoLogic)\
/* -- */\
COMPONENT_ENTRY1(NodeSelection)\
COMPONENT_ENTRY1(Resources)\
COMPONENT_ENTRY1(ShapeCanvas)\

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

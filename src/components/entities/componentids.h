#pragma once

// These macros should be used in class declarations.
// These IDs are used heavily in serialization and identifying
// components in Entities.

// We allows derived Components to form interfaces,
// or to implement other interfaces.
#define COMPONENT_ID(base,derived)\
  static size_t kIID()\
  { return ComponentIID::kI##base; }\
  \
  static size_t kDID()\
  { return ComponentDID::k##derived; }

// Component Interface IDs.
enum ComponentIID {
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
  kINodeGraphQuickItem,
  kINodeGraphRenderer,
  kIFileModel,
  kILinkGroup,
  kINodeGraphSelection,
  kIOutputPlug,
  kIResources,
  kIShapeCanvas,
  kITestComponent,
  // Count.
  kNumComponentInterfaceIDs,
};

// Component Derived/Implementation IDs.
enum ComponentDID {
  kInvalidComponent,
  // Factories
  kFactory,
  // CompShapes.
  kDotNodeShape,
  kGroupNodeShape,
  kInputNodeShape,
  kInputLabelShape,
  kInputShape,
  kLinkShape,
  kNodeShape,
  kOutputNodeShape,
  kOutputLabelShape,
  kOutputShape,
  // --
  kCompShapeCollective,
  // Computes.
  kDotNodeCompute,
  kInputCompute,
  kOutputCompute,
  kGroupNodeCompute,
  kInputNodeCompute,
  kScriptNodeCompute,
  kMockNodeCompute,
  kOutputNodeCompute,
  kOpenBrowserCompute,
  kCloseBrowserCompute,
  // --
  kFBORenderer,
  kFBOWorker,
  kGraphBuilder,
  // --
  kLowerHierarchyChange,
  kUpperHierarchyChange,
  // Interactions.
  kGroupInteraction,
  // --
  kNodeGraphEditor,
  kNodeGraphQuickItem,
  kNodeGraphRenderer,
  kFileModel,
  // --
  kNodeGraphSelection,
  kResources,
  kShapeCanvas,
  kTestComponent,
  // Count.
  kNumComponentImplementationIDs,
};

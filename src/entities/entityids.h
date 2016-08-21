#pragma once

// These macros should be used in class declarations.
// These IDs are used heavily in serialization and identifying
// components in Entities.

// Entities always implement the Entity class interface.
#define ENTITY_ID(derived, derived_name)\
  static size_t kIID()\
  { return EntityIID::kIEntity; }\
  virtual size_t get_iid() const\
  { return kIID(); }\
  \
  static size_t kDID()\
  { return EntityDID::k##derived; }\
  virtual size_t get_did() const\
  { return kDID(); }\
  \
  static const char* kDIDName()\
  { return derived_name; }\
  virtual const char* get_did_name() const\
  { return kDIDName(); }\


// Entity Interface IDs.
enum EntityIID {
  kIEntity,
};

// Entity Derived/Implementation IDs.
enum EntityDID {
  kInvalidEntity,
  // ------------------------------
  // Non Gui Based Entities.
  // ------------------------------
  kBaseNamespaceEntity,
  // App.
  kBaseAppEntity,
  // Links.
  kBaseLinkEntity,
  kBaseInputEntity,
  kBaseOutputEntity,
  // Group Node.
  kBaseGroupNodeEntity,
  kBaseInputNodeEntity,
  kBaseOutputNodeEntity,
  // Dot Node.
  kBaseDotNodeEntity,
  // Other Nodes.
  kBaseMockNodeEntity,
  kBaseComputeNodeEntity,
  // ------------------------------
  // Gui Based Entities.
  // ------------------------------
  // App.
  kAppEntity,
  kQMLAppEntity,
  kQtAppEntity,
  // Links.
  kLinkEntity,
  kInputEntity,
  kInputLabelEntity,
  kOutputEntity,
  kOutputLabelEntity,
  // Group Node.
  kGroupNodeEntity,
  kInputNodeEntity,
  kOutputNodeEntity,
  // Dot Node.
  kDotNodeEntity,
  // Other Nodes.
  kMockNodeEntity,
  kComputeNodeEntity,
  kOpenBrowserNodeEntity,
  kCloseBrowserNodeEntity,
  kCreateSetFromValuesNodeEntity,
  kCreateSetFromTypeNodeEntity,
  // Count.
  kNumEntityImplementationIDs,
};

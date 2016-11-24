#pragma once
#include <guicomponents/comms/comms_export.h>
#include <guicomponents/comms/commtypes.h>
#include <guicomponents/comms/hintkeys.h>

// Qt.
#include <QtCore/QObject>

namespace ngs {

// -------------------------------------------------------------------------------
// JSType.
// -------------------------------------------------------------------------------

class COMMS_EXPORT JSTypeWrap : public QObject {
Q_OBJECT
 public:
  JSTypeWrap()
      : QObject() {
  }

  QString get_string(int type) const {
    return js_type_to_string((JSType)type);
  }

  static const QStringList _gui_strings;
  Q_INVOKABLE const QStringList& get_gui_strings() {return _gui_strings;}


// Declare properties.
// Q_PROPERTY(int kObject READ kObject)
#undef COMPONENT_ENTRY1
#undef COMPONENT_ENTRY2
#define COMPONENT_ENTRY1(NAME) Q_PROPERTY(int k##NAME READ k##NAME CONSTANT)
#define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  JSTYPE_ENTRIES()


// Declare read members.
// int kObject(){return (int)JSType::kObject;}
#undef COMPONENT_ENTRY1
#undef COMPONENT_ENTRY2
#define COMPONENT_ENTRY1(NAME) int k##NAME(){return (int)JSType::k##NAME;}
#define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  JSTYPE_ENTRIES()
};

// -------------------------------------------------------------------------------
// HintKey.
// -------------------------------------------------------------------------------

class COMMS_EXPORT HintKeyWrap : public QObject {
Q_OBJECT
 public:
  HintKeyWrap()
      : QObject() {
  }

  QString get_string(int type) {
    return hint_key_to_string((HintKey)type);
  }

// Declare properties.
// Q_PROPERTY(int kObject READ kObject)
#undef COMPONENT_ENTRY1
#undef COMPONENT_ENTRY2
#define COMPONENT_ENTRY1(NAME) Q_PROPERTY(int k##NAME READ k##NAME CONSTANT)
#define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  HINTKEY_ENTRIES()

// Declare read members.
// int kObject(){return (int)JSType::kObject;}
#undef COMPONENT_ENTRY1
#undef COMPONENT_ENTRY2
#define COMPONENT_ENTRY1(NAME) int k##NAME(){return (int)HintKey::k##NAME;}
#define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  HINTKEY_ENTRIES()
};

// -------------------------------------------------------------------------------
// EnumHintValue.
// -------------------------------------------------------------------------------

class COMMS_EXPORT EnumHintValueWrap : public QObject {
Q_OBJECT
 public:
  EnumHintValueWrap()
      : QObject() {
  }

  QString get_string(int type) {
    return enum_hint_value_to_string((EnumHintValue)type);
  }

// Declare properties.
// Q_PROPERTY(int kObject READ kObject)

#undef COMPONENT_ENTRY1
#undef COMPONENT_ENTRY2
#define COMPONENT_ENTRY1(NAME) Q_PROPERTY(int k##NAME READ k##NAME CONSTANT)
#define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  ENUMHINTVALUE_ENTRIES()

// Declare read members.
// int kObject(){return (int)JSType::kObject;}

#undef COMPONENT_ENTRY1
#undef COMPONENT_ENTRY2
#define COMPONENT_ENTRY1(NAME) int k##NAME(){return (int)EnumHintValue::k##NAME;}
#define COMPONENT_ENTRY2(NAME, VALUE) COMPONENT_ENTRY1(NAME)
  ENUMHINTVALUE_ENTRIES()
};

// -------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------

}

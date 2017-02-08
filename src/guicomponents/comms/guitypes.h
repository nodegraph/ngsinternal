#pragma once
#include <guicomponents/comms/comms_export.h>

// Qt.
#include <QtCore/QObject>

namespace ngs {

class COMMS_EXPORT GUITypes : public QObject
{
    Q_OBJECT
 public:


    // Note these types are exposed to qml using Q_ENUMS.
    // Q_ENUMS requires that the enums begin with a capital letter.

    // ----------------------------------------------------------------------------------------------------------

    enum class JSType: int {
      Object = 0,
      Array = 1,
      String = 2,
      Number = 3,
      Boolean = 4,
      Null = 5,
      Undefined = 6
    };
    Q_ENUM(JSType)

    // ----------------------------------------------------------------------------------------------------------

    // Hints for numbers.
    // kEnumHint: If the js type is a number, then this represents the type of enum represented by an number javascript type.
    // kMinHint
    // kMaxHint
    //
    // Hints for elements in objects and arrays.
    // kElementJSTypeHint: This represents the javascript type of elements inside an array/object.
    // kElementEnumHint: This represents the type of enum represented by number elements inside an array/object.
    //
    // User info about the parameter.
    // kDescriptionHint: A helpful description of the parameter.
    //
    // Gui hints.
    // kMultiLineHint: Whether to use a single of multiline editor.


    enum class HintKey: int {
      EnumHint = 0,
      MinHint = 1,
      MaxHint = 2,
      ElementJSTypeHint = 3,
      ElementEnumHint = 4,
      DescriptionHint = 5,
      MultiLineHint = 6
    };
    Q_ENUM(HintKey)

    // ----------------------------------------------------------------------------------------------------------

    enum class EnumHintValue: int {
      NotEnumType = 0,
      MessageType = 1,
      InfoType = 2,
      MouseActionType = 3,
      TextActionType = 4,
      ElementActionType = 5,
      WrapType = 6,
      DirectionType = 7,
      HTTPSendType = 8,
      JSType = 9,
      PostType = 10
    };
    Q_ENUM(EnumHintValue)

    // ----------------------------------------------------------------------------------------------------------

    enum class PostType: int {
      Pass = 0,
      Fail = 1,
      Info = 2
    };
    Q_ENUM(PostType)

    GUITypes();
};

}

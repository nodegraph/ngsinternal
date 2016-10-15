import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

Item{
    id: app_enums
    objectName: "app_enums_object"
    visible: false

    function get_text(enum_type, index) {
        if (enum_type == 'MouseActionType') {
            return mouse_action_types[index]
        } else if (enum_type == 'TextActionType') {
            return text_action_types[index]
        } else if (enum_type == 'ElementActionType') {
            return element_action_types[index]
        } else if (enum_type == 'WrapType') {
            return wrap_types[index]
        } else if (enum_type == 'Direction') {
            return direction_types[index]
        } else {
            console.log('Error: attempt to edit an enum with invalid type.')
            console.log(new Error().stack);
        }
    }

    property var mouse_action_types: [
        'mouse click',
        'mouse over',
        'start mouse hover',
        'stop mouse hover'
    ]

    property var text_action_types: [
        'type letters',
        'type enter',
    ]

    property var element_action_types: [
        'extract text',
        'select an option',
        'scroll'
    ]

    property var wrap_types: [
        'text',
        'image',
        'input field',
        'select dropdown'
    ]

    property var direction_types: [
        'left',
        'right',
        'up',
        'down'
    ]

}

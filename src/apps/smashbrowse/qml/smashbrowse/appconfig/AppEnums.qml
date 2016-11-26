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

    // --------------------------------------------------------------------------------------
    // Javascript Type.
    // --------------------------------------------------------------------------------------

    // Get the value type at the given path in _values.
    function determine_js_type(value) {
        if (value === undefined) {
            return js_type.kUndefined
        } else if (value === null) {
            return js_type.kNull
        }else if (typeof value === 'string') {
            // String.
            return js_type.kString
        } else if (typeof value === 'boolean') {
            // Boolean.
            return js_type.kBoolean
        } else if (typeof value === 'number'){
            // Number.
            return js_type.kNumber
        } else if (typeof value === 'object') {
            if (Object.getPrototypeOf(value) === Object.prototype) {
                // Object.
                return js_type.kObject
            } else if (Object.getPrototypeOf(value) === Array.prototype) {
                // Array.
                return js_type.kArray
            }
        }
        return js_type.kUndefined
    }

    // --------------------------------------------------------------------------------------
    // Msg Enum Enum. This is used to add additional info (as in hint) to an integer.
    // This allows us to display a custom dropdown in the gui for the integer as an enum.
    // --------------------------------------------------------------------------------------

    function get_enum_hint_value_text(enum_hint_value_type, index) {
        if (enum_hint_value_type == enum_hint_value.kMouseActionType) {
            return mouse_action_type_text[index]
        } else if (enum_hint_value_type == enum_hint_value.kTextActionType) {
            return text_action_type_text[index]
        } else if (enum_hint_value_type == enum_hint_value.kElementActionType) {
            return element_action_type_text[index]
        } else if (enum_hint_value_type == enum_hint_value.kWrapType) {
            return wrap_type_text[index]
        } else if (enum_hint_value_type == enum_hint_value.kDirectionType) {
            return direction_type_text[index]
        } else if (enum_hint_value_type == enum_hint_value.kHTTPSendType) {
        	return http_send_type_text[index]
        } else if (enum_hint_value_type == enum_hint_value.kJSType) {
        	return js_type_text[index]
        } else {
            console.log('Error: attempt to get text for an enum with invalid type.')
            console.log(new Error().stack);
        }
    }

    property var mouse_action_type_text: [
        'mouse click',
        'mouse over',
        'start mouse hover',
        'stop mouse hover'
    ]

    property var text_action_type_text: [
        'type letters',
        'type enter',
    ]

    property var element_action_type_text: [
        'extract text',
        'select an option',
        'scroll'
    ]

    property var wrap_type_text: [
        'text',
        'image',
        'input field',
        'select dropdown'
    ]

    property var direction_type_text: [
        'left',
        'right',
        'up',
        'down'
    ]
    
    property var http_send_type_text: [
      'get',
	  'post',
	  'put',
	  'delete',
	  'patch'
    ]
    
    property var js_type_text: [
  		'object',
  		'array',
  		'string',
  		'number',
  		'boolean',
    ]

}

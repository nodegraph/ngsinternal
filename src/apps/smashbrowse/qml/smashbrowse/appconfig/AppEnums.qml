import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import JSTypeWrap 1.0

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
        	console.log('111')
            return js_type.kUndefined
        } else if (value === null) {
        	console.log('222')
            return js_type.kNull
        }else if (typeof value === 'string') {
            // String.
            console.log('333')
            return js_type.kString
        } else if (typeof value === 'boolean') {
            // Boolean.
            console.log('444')
            return js_type.kBoolean
        } else if (typeof value === 'number'){
            // Number.
            console.log('555')
            return js_type.kNumber
        } else if (typeof value === 'object') {
            if (Object.getPrototypeOf(value) === Object.prototype) {
                // Object.
                console.log('666')
                return js_type.kObject
            } else if (Object.getPrototypeOf(value) === Array.prototype) {
                // Array.
                console.log('777')
                return js_type.kArray
            }
        }
        console.log('888')
        return js_type.kUndefined
    }

    // --------------------------------------------------------------------------------------
    // Msg Enum Enum. This is used to add additional info (as in hint) to an integer.
    // This allows us to display a custom drop down in the gui for the integer as an enum.
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
            console.log('Error: attempt to get text for an enum with invalid type: ' + enum_hint_value_type)
            console.log(new Error().stack);
        }
    }

    property var mouse_action_type_text: [
        'mouse click',
        'mouse ctrl click',
        'mouse over'
    ]

    property var text_action_type_text: [
        'type letters',
        'press enter',
    ]

    property var element_action_type_text: [
        'extract text',
        'choose an option',
        'scroll'
    ]

    property var wrap_type_text: [
        'text',
        'image',
        'input field',
        'drop down field'
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

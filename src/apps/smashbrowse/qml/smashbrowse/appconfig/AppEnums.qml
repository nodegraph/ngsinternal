import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import GUITypes 1.0

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
            return GUITypes.Undefined
        } else if (value === null) {
            return GUITypes.Null
        }else if (typeof value === 'string') {
            // String.
            return GUITypes.String
        } else if (typeof value === 'boolean') {
            // Boolean.
            return GUITypes.Boolean
        } else if (typeof value === 'number'){
            // Number.
            return GUITypes.Number
        } else if (typeof value === 'object') {
            if (Object.getPrototypeOf(value) === Object.prototype) {
                // Object.
                return GUITypes.Object
            } else if (Object.getPrototypeOf(value) === Array.prototype) {
                // Array.
                return GUITypes.Array
            }
        }
        return GUITypes.Undefined
    }

    // --------------------------------------------------------------------------------------
    // Msg Enum Enum. This is used to add additional info (as in hint) to an integer.
    // This allows us to display a custom drop down in the gui for the integer as an enum.
    // --------------------------------------------------------------------------------------

    function get_enum_hint_value_text(enum_hint_value_type, index) {
        if (enum_hint_value_type == GUITypes.MouseActionType) {
            return mouse_action_type_text[index]
        } else if (enum_hint_value_type == GUITypes.TextActionType) {
            return text_action_type_text[index]
        } else if (enum_hint_value_type == GUITypes.ElementActionType) {
            return element_action_type_text[index]
        } else if (enum_hint_value_type == GUITypes.WrapType) {
            return wrap_type_text[index]
        } else if (enum_hint_value_type == GUITypes.DirectionType) {
            return direction_type_text[index]
        } else if (enum_hint_value_type == GUITypes.HTTPSendType) {
        	return http_send_type_text[index]
        } else if (enum_hint_value_type == GUITypes.JSType) {
        	return js_type_text[index]
        } else if (enum_hint_value_type == GUITypes.PostType) {
        	return post_type_text[index]
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
    
    property var post_type_text: [
  		'pass',
  		'fail',
  		'info',
    ]

}

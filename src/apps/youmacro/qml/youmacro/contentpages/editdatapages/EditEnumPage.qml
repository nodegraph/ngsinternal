import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import youmacro.appwidgets 1.0

import GUITypes 1.0

BaseEditPage {
    id: page

	// Our Methods.
	
    // Note the enum type should be set before this.
    function set_value(value) {
        if (value >= value_editor_loader.item.count || value < 0) {
            console.log('Error: attempt to set a value that is out of range for this enum type.')
            console.log(new Error().stack);
        }
        value_editor_loader.item.currentIndex = value
    }
    
    function get_value(value) {
        return value_editor_loader.item.currentIndex
    }
    
    function set_enum_type(type) {
    	var cb = value_editor_loader.item
        if (type == GUITypes.MouseActionType) {
            console.log('edit setting options: ' + app_enums.mouse_action_type_text)
            cb.set_option_texts(app_enums.mouse_action_type_text)
        } else if (type == GUITypes.TextActionType) {
            console.log('edit setting options: ' + app_enums.text_action_type_text)
            cb.set_option_texts(app_enums.text_action_type_text)
        } else if (type == GUITypes.ElementActionType) {
            console.log('edit setting options: ' + app_enums.element_action_type_text)
            cb.set_option_texts(app_enums.element_action_type_text)
        } else if (type == GUITypes.ElementType) {
            console.log('edit setting options: ' + app_enums.element_type_text)
            cb.set_option_texts(app_enums.element_type_text)
        } else if (type == GUITypes.DirectionType) {
            console.log('edit setting options: ' + app_enums.direction_type_text)
            cb.set_option_texts(app_enums.direction_type_text)
        } else if (type == GUITypes.HTTPSendType) {
            console.log('edit setting options: ' + app_enums.http_send_type_text)
            cb.set_option_texts(app_enums.http_send_type_text)
        } else if (type == GUITypes.JSType) {
        	console.log('edit setting options: ' + app_enums.js_type_text)
            cb.set_option_texts(app_enums.js_type_text)
        } else if (type == GUITypes.PostType) {
        	console.log('edit setting options: ' + app_enums.post_type_text)
        	cb.set_option_texts(app_enums.post_type_text)
        } else {
            console.log('Error: attempt to edit an enum with invalid type.')
            console.log(new Error().stack);
        }
    }

	value_editor_component: Component {
        AppComboBox {
        	width: app_settings.page_content_width
	    	Layout.maximumWidth: app_settings.page_content_width
        }
	}
        
}



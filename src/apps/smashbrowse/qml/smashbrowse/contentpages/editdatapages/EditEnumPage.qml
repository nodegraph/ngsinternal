import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import smashbrowse.appwidgets 1.0

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
        if (type == enum_hint_value.kMouseActionType) {
            console.log('edit setting options: ' + app_enums.mouse_action_type_text)
            cb.set_option_texts(app_enums.mouse_action_type_text)
        } else if (type == enum_hint_value.kTextActionType) {
            console.log('edit setting options: ' + app_enums.text_action_type_text)
            cb.set_option_texts(app_enums.text_action_type_text)
        } else if (type == enum_hint_value.kElementActionType) {
            console.log('edit setting options: ' + app_enums.element_action_type_text)
            cb.set_option_texts(app_enums.element_action_type_text)
        } else if (type == enum_hint_value.kWrapType) {
            console.log('edit setting options: ' + app_enums.wrap_type_text)
            cb.set_option_texts(app_enums.wrap_type_text)
        } else if (type == enum_hint_value.kDirectionType) {
            console.log('edit setting options: ' + app_enums.direction_type_text)
            cb.set_option_texts(app_enums.direction_type_text)
        } else if (type == enum_hint_value.kHTTPSendType) {
            console.log('edit setting options: ' + app_enums.http_send_type_text)
            cb.set_option_texts(app_enums.http_send_type_text)
        } else if (type == enum_hint_value.kJSType) {
        	console.log('edit setting options: ' + app_enums.js_type_text)
            cb.set_option_texts(app_enums.js_type_text)
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



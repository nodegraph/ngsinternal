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
    function set_value(value) {
        value_editor_loader.item.set_text(value)
    }

    function get_value(value) {
        return value_editor_loader.item.get_text()
    }
    
    value_editor_component: Component {
        AppMultiLineEdit {
        	// This height value is a hack for now as setting this to app_settings.page_height/3 
        	// results in the editor now showing properly.
        	height: app_units.dp(300) 
        	width: app_settings.page_content_width
	    	Layout.maximumWidth: app_settings.page_content_width
        }
	} 
}



import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import robodownloader.appwidgets 1.0

BaseEditPage {
    id: page

    // Our Methods.
    function set_value(value) {
        value_editor_loader.item.check_box.checked = value
    }

    function get_value(value) {
        return value_editor_loader.item.check_box.checked
    }
    
   	value_editor_component: Component {
   		id: value_editor_component
   		
	    RowLayout {
	    	property alias check_box: check_box
	    	width: app_settings.page_content_width
	    	Layout.maximumWidth: app_settings.page_content_width
	    	Item {Layout.fillWidth: true}
	    	AppLabel {
	    		text: qsTr("check if true")
	    	}
	    	Rectangle {
	            color: "transparent"
	            height: app_settings.action_bar_height
	            width: app_settings.button_spacing
	        }
	        AppCheckBox {
	            id: check_box
	            checked: false
	        }
	        Item {Layout.fillWidth: true}
	    }
	}
        
}



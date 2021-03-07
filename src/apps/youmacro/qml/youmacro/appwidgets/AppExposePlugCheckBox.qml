import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import youmacro.appwidgets 1.0

RowLayout {
	visible: true
	Layout.maximumWidth: parent.width
	
	function set_exposed(exposed) {
    	exposed_check_box.checked = exposed
    }
    
    function get_exposed() {
    	return exposed_check_box.checked
    }
	
	
	Item {Layout.fillWidth: true}
	AppLabel {
		text: qsTr("expose as a plug in the node graph")
	}
	Rectangle {
        color: "transparent"
        height: app_settings.action_bar_height
        width: app_settings.button_spacing
    }
    AppCheckBox {
        id: exposed_check_box
        checked: false
    }
    Item {Layout.fillWidth: true}
}
import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import youmacro.appwidgets 1.0

BaseEditPage {
	id: page
	
    // Our Methods.
    function set_value(value) {
        value_editor_loader.item.text = value
    }

    function get_value(value) {
        return Number(value_editor_loader.item.text)
    }
    
	value_editor_component: Component {
        AppLineEdit {
            tool_bar: copy_paste_bar
            text: "0"
            width: app_settings.page_content_width
            validator: IntValidator {
            				bottom: 0
            			}
        }
	}
}



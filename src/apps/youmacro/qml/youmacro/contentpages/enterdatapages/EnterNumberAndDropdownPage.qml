import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import youmacro.appwidgets 1.0

Rectangle {
    id: page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Appearance.
    color: app_settings.menu_stack_bg_color

    // Callback will be called with the entered text.
    property var callback

	property var option_ids

    // Our Methods.
    function set_bottom_value(bottom) {
    	text_field.validator.bottom = bottom
    }
    
    function set_top_value(top) {
    	text_field.validator.top = top
    }
    
    function set_value(value) {
        text_field.text = value.toString()
    }

	function get_value() {
		return Number(text_field.text)
	}
    
    function set_title(title) {
        stack_view_header.title_text = title
    }
    function get_title() {
        return stack_view_header.title_text
    }
    
    function set_description(desc) {
        description.text = desc
    }
    
    function set_option_texts(otexts) {
        combo_box.set_option_texts(otexts);
    }
    
    function set_option_ids(ids) {
    	option_ids = ids
    }

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        stack_view: page.Stack.view
    }

    ColumnLayout {
        y: app_settings.action_bar_height

        //height: app_settings.menu_page_height
        width: app_settings.menu_page_width
        spacing: app_settings.column_layout_spacing
        
        anchors {
	        left: parent.left
	        right: parent.right
	        leftMargin: app_settings.page_left_margin
	        rightMargin: app_settings.page_right_margin
        }

        AppSpacer {}
        
        AppText  {
            id: description
            anchors.horizontalCenter: parent.horizontalCenter // used when the text is actually a single line
            Layout.maximumWidth: parent.width
        }
        
        AppSpacer {}

        // Text Field.
        AppLineEdit {
            id: text_field
            tool_bar: copy_paste_bar
            text: "0"
            anchors {
                left: parent.left
                right: parent.right
            }
            validator: IntValidator {
            				bottom: 0
            			}
        }
        
        AppSpacer {}
        
        AppComboBox {
            id: combo_box
            anchors {
			    left: parent.left
			    right: parent.right
			    leftMargin: app_settings.page_left_margin
			    rightMargin: app_settings.page_right_margin
			}
        }
        
        AppSpacer {}

        // Buttons.
        RowLayout {
        	Layout.maximumWidth: parent.width
        	
            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "accept"
                onClicked: {
                	page.Stack.view.pop_page()
                    page.callback(option_ids[combo_box.currentIndex], get_value())
                }
            }
            Rectangle {
                color: "transparent"
                height: app_settings.action_bar_height
                width: app_settings.button_spacing
            }
            AppLabelButton {
                text: "cancel"
                onClicked: {
                    page.Stack.view.pop_page()
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}



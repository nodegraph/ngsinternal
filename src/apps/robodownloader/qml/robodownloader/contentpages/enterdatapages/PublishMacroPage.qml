import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import robodownloader.appwidgets 1.0

Rectangle {
    id: page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0
    
    // Properties.
    color: app_settings.menu_stack_bg_color

    // Properties.
    property bool create_file: true
    property alias name_field: name_field
    property bool private_macro: true

    // Our Methods.
    function set_title(title) {
        stack_view_header.title_text = title
    }
    
    function set_overwrite_macros(overwrite) {
    	overwrite_check_box.checked = overwrite
    }

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        stack_view: page.Stack.view
    }

    ColumnLayout {
        // Geometry.
        height: app_settings.menu_page_height / 2
        width: app_settings.menu_page_width

        // Positioning.
        x: 0
        y: app_settings.action_bar_height
        z: 0

        // Appearance.
        spacing: app_settings.column_layout_spacing


        ColumnLayout {
            spacing: 0
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: app_settings.page_left_margin
                rightMargin: app_settings.page_right_margin
            }
            
            AppSpacer {}

            AppLabel {
                id : title_label
                text: "Macro Name"
                anchors {
                    //left: parent.left
                    //right: parent.right
                    //leftMargin: app_settings.page_left_margin
                	//rightMargin: app_settings.page_right_margin
                	horizontalCenter: parent.horizontalCenter
                }
            }
            
            AppSpacer {}

            // Title Field.
            AppLineEdit {
                id: name_field
                tool_bar: copy_paste_bar
                text: "some_macro"
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }
            
            AppSpacer {}
        }
        
        RowLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            AppLabel {
                id: overwrite_label
                text: "overwrite existing macros "
                anchors {
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }
            AppCheckBox {
                id: overwrite_check_box
                checked: false
                anchors {
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
                onClicked: status_label.text = ""
            }
        }

        // Buttons.
        RowLayout {
        	Layout.maximumWidth: parent.width
        
            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "accept"
                onClicked: {
                    var macro_name = name_field.text
                    
                    if (overwrite_check_box.checked) {
                    	if (private_macro) {
                    		file_model.publish_private_macro(macro_name)
                    	} else {
                    		file_model.publish_public_macro(macro_name)
                    	}
                    	page.Stack.view.pop_page()
                    } else {
                    	if (private_macro) {
                    		if (!file_model.private_macro_exists(macro_name)) {
                    			file_model.publish_private_macro(macro_name)
	                    		page.Stack.view.pop_page()
                    		} else {
	                    		status_label.text = "a macro with the same name exists already"
	                    		update()
	                    	}
                    	} else {
                    		if (!file_model.public_macro_exists(macro_name)) {
                    			file_model.publish_public_macro(macro_name)
	                    		page.Stack.view.pop_page()
                    		} else {
	                    		status_label.text = "a macro with the same name exists already"
	                    		update()
	                    	}
                    	}
                    }
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
        
        // Shows status of publishing.
        Label {
            id: status_label
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
            font.pointSize: app_settings.font_point_size
            font.bold: false
            font.italic: false
            color: "yellow"

            function on_mouse_pressed() {
                text = "processing ..."
                update()
            }
        }
    }
}



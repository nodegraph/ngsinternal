import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import smashbrowse.appwidgets 1.0

Rectangle {
    id: check_password_page

    // Dimensions.
    height: app_settings.screen_height
    width: app_settings.screen_width

    // Positioning.
    x: 0
    y: 0
    z: 10

    // Properties.
    color: app_settings.menu_stack_bg_color
    
    onVisibleChanged: if (visible) password_field.forceActiveFocus()

	function initiate_license_check() {
		status_label.on_mouse_pressed()
		update()
		if (crypto_logic.check_password(password_field.text)) {
        	license_checker.load()
            app_utils.check_license(on_license_checked)
        } else {
            status_label.text = "password is incorrect"
        }
	}
    function on_license_checked(valid) {
    	// Invalid License.
    	if (!valid) {
    	    // Hide this page.
	        check_password_page.visible = false
	
	        // Show the license page.
	        license_page.visible = true
	
	        // Erase the password from this page.
	        password_field.text = ""
    		return
    	}
    	
    	// Valid License.
    	
        // Hide this page.
        check_password_page.visible = false

        if (!upgrade_license_check_box.checked) {
            // Erase the password from this page.
            password_field.text = ""
            
            // Switch to node graph mode.
            main_bar.switch_to_node_graph()

            // Load the last graph.
            app_utils.load_last_graph()
        } else {
            license_page.update_fields()
            license_page.visible = true
        }
    }

    ColumnLayout {
        height: app_settings.screen_height
        width: app_settings.screen_width
        spacing: app_settings.column_layout_spacing * 2

        // Filler.
        Item {Layout.fillHeight: true}

        // Password lablel.
        Label {
            id: app_label
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Password"
            font.pointSize: app_settings.large_font_point_size
            font.bold: false
            font.italic: false
            color: "white"
        }

        // Password field.
        AppPasswordField {
            id: password_field
            echoMode: TextInput.Password
            anchors.horizontalCenter: parent.horizontalCenter
        }

        RowLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            AppLabel {
                id: upgrade_license_label
                text: "upgrade license "
                anchors {
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }
            AppCheckBox {
                id: upgrade_license_check_box
                checked: false
                anchors {
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }
        }

        // Continue button.
        AppLabelButton {
            id: continue_button
            anchors.horizontalCenter: parent.horizontalCenter
            text: "continue"
            onClicked: initiate_license_check()
        }

        // Shows status of password processing.
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

        // Filler.
        Item {Layout.fillHeight: true}

        // Hook up our signals.
        Component.onCompleted: {
            continue_button.mouse_pressed.connect(status_label.on_mouse_pressed)
            password_field.return_pressed_callback = status_label.on_mouse_pressed
            password_field.return_released_callback = check_password_page.initiate_license_check
        }
    }
}



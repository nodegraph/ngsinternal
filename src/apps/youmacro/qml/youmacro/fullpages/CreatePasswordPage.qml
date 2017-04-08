import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import youmacro.appwidgets 1.0

Rectangle {
    id: create_password_page
    
    onVisibleChanged: if (visible) password_1.password_field.forceActiveFocus()

    // Dimensions.
    height: app_settings.screen_height
    width: app_settings.screen_width

    // Positioning.
    x: 0
    y: 0
    z: 10

    // Properties.
    color: app_settings.menu_stack_bg_color
    
    function wipe_passwords() {
    	// Erase passwords from page.
        password_1.text = ""
        password_2.text = ""
    }

    function show_license_entry_page() {
        // Hide this page.
        create_password_page.visible = false

        // Show the license page.
        license_page.update_fields()
        license_page.visible = true
    }
    
    function check_passwords_match() {
	    if (password_1.text != password_2.text) {
	        status.text = "passwords do not match"
	    } else if ((!password_1.text) || (!password_2.text)) {
	        status.text = "passwords cannot be empty"
	    } else {
	        initiate_license_check()
	    }
    }
    
    function initiate_license_check() {
	    // Setup the crypto.
	    crypto_logic.create_crypto(password_1.text)
	    crypto_logic.save_crypto();
	
	    // The first the app is run, there won't be a license file.
	    // The license will automatically go to the "lite" edition with un unspecified license key.
	    license_checker.load()
	    app_utils.check_license(on_license_checked)
    }
    
    function on_license_checked(valid) {
    	wipe_passwords()
    	if (valid) {
    		license_checker.save()
    		create_password_page.visible = false
    		
	        // Show the first page.
			main_bar.switch_to_first_page()
			// Load the last graph.
			app_utils.load_last_graph()
		} else {
			show_license_entry_page()
		}
	}
	
	Keys.onPressed: {
    	if ((event.key == Qt.Key_Return) || (event.key == Qt.Key_Enter)) {
    		status.on_mouse_pressed()
        }
    }
    Keys.onReleased: {
        if ((event.key == Qt.Key_Return) || (event.key == Qt.Key_Enter)) {
        	check_passwords_match()
        }
    }
    
    ColumnLayout {
        height: app_settings.screen_height
        width: app_settings.screen_width
        spacing: app_settings.column_layout_spacing * 2

        // Filler.
        Item {Layout.fillHeight: true}

        // Label for first password entry.
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Create a password"
            font.pointSize: app_settings.font_point_size
            font.bold: false
            font.italic: false
            color: "white"
        }

        // First password entry.
        AppPasswordField {
            id: password_1
            reveal_check_box_visible: false
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Label for second password entry.
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Confirm your password"
            font.pointSize: app_settings.font_point_size
            font.bold: false
            font.italic: false
            color: "white"
        }

        // Second password entry.
        AppPasswordField {
            id: password_2
            reveal_check_box_visible: false
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Continue button.
        AppLabelButton {
            id: continue_button
            anchors.horizontalCenter: parent.horizontalCenter
            text: "continue"
            onClicked: {
            	check_passwords_match()
            }
        }

        // Shows status of password processing.
        Label {
            id: status
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
            continue_button.mouse_pressed.connect(status.on_mouse_pressed)
        }
    }
}



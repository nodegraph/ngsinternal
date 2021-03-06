import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import youmacro.appwidgets 1.0

Rectangle {
    id: check_password_page
    onVisibleChanged: if (visible) password_field.password_field.forceActiveFocus()

    // Dimensions.
    height: app_settings.screen_height
    width: app_settings.screen_width

    // Positioning.
    x: 0
    y: 0
    z: 10

    // Properties.
    color: app_settings.menu_stack_bg_color
    property bool update_license: false
    
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

        if (!update_license) {
            // Erase the password from this page.
            password_field.text = ""
            
            // Show the first page.
            main_bar.switch_to_first_page()

            // Load the last graph.
            app_utils.load_last_graph()
        } else {
            license_page.update_fields()
            license_page.visible = true
        }
    }
    
    Keys.onPressed: {
    	if ((event.key == Qt.Key_Return) || (event.key == Qt.Key_Enter)) {
    		status_label.on_mouse_pressed()
        }
    }
    Keys.onReleased: {
        if ((event.key == Qt.Key_Return) || (event.key == Qt.Key_Enter)) {
        	initiate_license_check()
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
        
        AppSpacer{}

	    // Password field.
	    AppPasswordField {
	        id: password_field
	        reveal_check_box_visible: false
	        anchors.horizontalCenter: parent.horizontalCenter
	    }
	    
	    AppSpacer{}
		    
        // Continue button.
        //AppLabelButton {
        //    id: update_license_button
        //    anchors.horizontalCenter: parent.horizontalCenter
        //    text: "update license"
        //    onClicked: {
	    //        	update_license = true 
	    //        	initiate_license_check()
        //    	}
        //}        

        // Continue button.
        AppLabelButton {
            id: continue_button
            anchors.horizontalCenter: parent.horizontalCenter
            text: "login"
            onClicked: {
	            	update_license = false
	            	initiate_license_check()
            	}
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
        }
    }
}



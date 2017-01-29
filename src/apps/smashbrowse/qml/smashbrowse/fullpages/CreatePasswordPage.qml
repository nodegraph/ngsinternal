import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import smashbrowse.appwidgets 1.0

Rectangle {
    id: create_password_page

    // Dimensions.
    height: app_settings.screen_height
    width: app_settings.screen_width

    // Positioning.
    x: 0
    y: 0
    z: 10

    // Properties.
    color: app_settings.menu_stack_bg_color

    function show_license_entry_page() {
        // Hide this page.
        create_password_page.visible = false

        // Show the license page.
        license_page.update_fields()
        license_page.visible = true

        // Erase passwords from page.
        password_1.text = ""
        password_2.text = ""
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
            echoMode: TextInput.Password
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
            echoMode: TextInput.Password
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Continue button.
        AppLabelButton {
            id: continue_button
            anchors.horizontalCenter: parent.horizontalCenter
            text: "continue"
            onClicked: {
                if (password_1.text != password_2.text) {
                    status.text = "passwords do not match"
                } else if ((password_1.length == 0) || (password_2.length == 0)) {
                    status.text = "passwords cannot be empty"
                } else {
                    // Setup the crypto.
                    crypto_logic.create_crypto(password_1.text)
                    crypto_logic.save_crypto();

                    // Now show the license entry page.
                    show_license_entry_page()
                }
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



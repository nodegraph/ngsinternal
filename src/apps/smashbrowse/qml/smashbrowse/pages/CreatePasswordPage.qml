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

    function on_nodejs_connected() {
        // Hide this page.
        create_password_page.visible = false

        // Show the license page.
        license_page.visible = true
        app_comm.nodejs_connected.disconnect(on_nodejs_connected)

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
                    file_model.create_crypto(password_1.text)
                    file_model.save_crypto();

                    // Connect to nodejs.
                    app_utils.connect_to_nodejs(on_nodejs_connected)
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



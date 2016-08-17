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

    function on_nodejs_connected() {
        console.log('checking password: ' + file_model.get_license())
        app_utils.check_license(file_model.get_edition(), file_model.get_license(), on_valid_license, on_invalid_license)
    }

    function on_valid_license() {
        // Hide this page.
        check_password_page.visible = false

        if (!upgrade_license_check_box.checked) {
            // Switch to node graph mode.
            main_bar.on_switch_to_mode(app_settings.node_graph_mode)
            app_comm.show_browser()

            // Erase the password from this page.
            password_field.text = ""

            // Load the last graph.
            app_utils.load_last_graph()
        } else {
            license_page.update_fields()
            license_page.visible = true
        }
    }

    function on_invalid_license() {
        // Hide this page.
        check_password_page.visible = false

        // Show the license page.
        license_page.visible = true

        // Erase the password from this page.
        password_field.text = ""
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
            anchors.horizontalCenter: parent.horizontalCenter
        }

        RowLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            AppLabel {
                id: upgrade_license_label
                text: "upgrade license? "
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
            onClicked: {
                if (file_model.check_password(password_field.text)) {
                    app_utils.connect_to_nodejs(on_nodejs_connected)
                } else {
                    status_label.text = "password is incorrect"
                }
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



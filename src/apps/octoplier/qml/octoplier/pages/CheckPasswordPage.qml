import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.appwidgets 1.0

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
            id: password
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Continue button.
        AppLabelButton {
            id: continue_button
            anchors.horizontalCenter: parent.horizontalCenter
            text: "continue"
            onClicked: {
                if (file_model.check_password(password.text)) {
                    // Load the last node graph.
                    file_model.load_model()
                    file_model.load_graph()
                    node_graph_page.node_graph.update()
                    // Frame everything.
                    node_graph_page.node_graph.frame_all()
                    node_graph_page.node_graph.update()
                    // Hide this page and erase password from page.
                    check_password_page.visible = false
                    password.text = ""
                    // Switch to node graph mode.
                    main_bar.on_switch_to_mode(app_settings.node_graph_mode)
                    Qt.inputMethod.hide()
                    app_comm.start_polling()
                } else {
                    status.text = "password is incorrect"
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



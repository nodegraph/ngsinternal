import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.tools 1.0

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
    color: app_settings.ng_bg_color

    ColumnLayout {
        height: app_settings.screen_height
        width: app_settings.screen_width
        spacing: app_settings.column_layout_spacing

        Item {Layout.fillHeight: true}

        AppLabel {
            text: "Create password"
        }

        // URL Text Entry Field.
        AppTextField {
            id: password_1
            tool_bar: copy_paste_bar
            inputMethodHints: Qt.ImhPreferLowercase | Qt.ImhSensitiveData | Qt.ImhHiddenText
            echoMode: TextInput.Password
            text: ""
            onAccepted: {
            }
        }

        AppLabel {
            text: "Confirm password"
        }

        // URL Text Entry Field.
        AppTextField {
            id: password_2
            tool_bar: copy_paste_bar
            inputMethodHints: Qt.ImhPreferLowercase | Qt.ImhSensitiveData | Qt.ImhHiddenText
            echoMode: TextInput.Password
            text: ""
            onAccepted: {
            }
        }

        // Buttons.
        RowLayout {
            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "create"
                onClicked: {
                    if (password_1.text != password_2.text) {
                        message_dialog.title = "Password Mismatch"
                        message_dialog.icon = StandardIcon.Critical
                        message_dialog.show("The passwords do not match.")
                    } else if (password_1.text == "") {
                        message_dialog.title = "Password Disallowed"
                        message_dialog.icon = StandardIcon.Critical
                        message_dialog.show("Empty passwords are not allowed.")
                    } else {
                        file_model.create_crypto(password_1.text)
                        file_model.load_model()
                        file_model.load_graph()
                        node_graph_page.node_graph.update()
                        create_password_page.visible = false
                        main_bar.on_switch_to_mode(app_settings.node_graph_mode)
                    }
                }
            }
            Item {Layout.fillWidth: true}
        }

        Item {Layout.fillHeight: true}
    }
}



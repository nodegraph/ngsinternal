import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.tools 1.0

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
    color: app_settings.ng_bg_color

    ColumnLayout {
        height: app_settings.screen_height
        width: app_settings.screen_width
        spacing: app_settings.column_layout_spacing

        // Logo.
//        Image {
//            id: splash_image
//            width: parent.width / 3
//            height: width
//            x: width
//            y: (parent.height - width) / 2
//            source: "qrc:///images/octopus_white.png"
//        }

        Item {Layout.fillHeight: true}

        AppLabel {
            text: "Enter password"
        }

        // URL Text Entry Field.
        AppTextField {
            id: password
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
                text: "ok"
                onClicked: {
                    if (file_model.check_password(password.text)) {
                        file_model.load_model()
                        file_model.load_graph()
                        node_graph_page.node_graph.update()
                        check_password_page.visible = false
                        main_bar.on_switch_to_mode(app_settings.node_graph_mode)
                    } else {
                        message_dialog.title = "Incorrect Password"
                        message_dialog.icon = StandardIcon.Critical
                        message_dialog.show("The password entered is incorrect.")
                    }
                }
            }
            Item {Layout.fillWidth: true}
        }

        Item {Layout.fillHeight: true}
    }
}



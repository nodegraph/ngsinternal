import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0
import Qt.labs.settings 1.0

import smashbrowse.stackedmodepages 1.0
import smashbrowse.appwidgets 1.0


RadioButton {
    id: app_radio_button
    text: "hello"
    style: RadioButtonStyle {
            indicator: Rectangle {
                    implicitWidth: app_settings.check_box_size
                    implicitHeight: app_settings.check_box_size
                    radius: app_settings.check_box_size /2
                    border.color: app_settings.prompt_color
                    border.width: app_settings.check_box_border_width
                    color: app_settings.menu_stack_bg_color
                    Rectangle {
                        anchors.fill: parent
                        visible: control.checked
                        color: "white"
                        radius: app_settings.check_box_size /2
                        anchors.margins: app_settings.check_box_border_width
                    }
            }
            label: AppLabel {
                id: radio_label
                text: app_radio_button.text
            }
        }
}

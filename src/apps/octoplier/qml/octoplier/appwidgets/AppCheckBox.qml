import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0
import Qt.labs.settings 1.0

import octoplier.stackedpages 1.0
import octoplier.appwidgets 1.0

CheckBox {
    id: check_box
    style: CheckBoxStyle {
        indicator: Rectangle {
                implicitWidth: app_settings.check_box_size
                implicitHeight: app_settings.check_box_size
                radius: app_settings.check_box_radius
                border.color: 'white'
                border.width: app_settings.check_box_border_width
                color: app_settings.prompt_color
                Rectangle {
                    visible: control.checked
                    radius: app_settings.check_box_radius
                    border.color: 'white'
                    border.width: app_settings.check_box_border_width
                    color: app_settings.prompt_color
                    //anchors.margins: app_settings.check_box_border_width
                    anchors.fill: parent

                    Text {
                        text: 'X'
                        font.family: "Arial"
                        font.bold: true
                        font.pointSize: app_settings.menu_page_title_point_size
                        font.italic: false
                        color: "white"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
        }
    }
}

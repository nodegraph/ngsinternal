import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import octoplier.config 1.0
import octoplier.menus 1.0
import octoplier.tools 1.0
import octoplier.testdata 1.0


ScrollView {
    id: menu_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    // Scrolling.
    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
    verticalScrollBarPolicy: (Qt.platform.os == "android") || (Qt.platform.os == "ios") ? Qt.ScrollBarAlwaysOff : Qt.ScrollBarAsNeeded

    // Dependencies.
    property var copy_paste_bar

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.settings_mode) {
            visible = true;
        } else {
            visible = false;
        }
    }

    Rectangle {

        // Dimensions.
        height: app_settings.page_height
        width: app_settings.page_width

        // Positioning.
        x: app_settings.page_x
        y: 0
        z: app_settings.page_z

        color: app_settings.ng_bg_color

        Component {
          id: list_view_header
          Rectangle {
            width: app_settings.page_width
            height: app_settings.list_item_height_small
            color:  app_settings.tree_view_selected_row_bg_color
            Text {
                text: '<b>' + 'Value Operators' + '</b>'
                font.pointSize: app_settings.font_point_size
                font.italic: false
                color: "white"
            }
          }
        }

        Component {
            id: list_view_delegate
            Rectangle {
                width: parent.width;
                height: app_settings.list_item_height_large
                border.color: "white"
                border.width: app_settings.list_item_border_width
                color: "transparent"
                Row {
                    Rectangle {
                        height: app_settings.list_item_height_large
                        width: app_settings.list_item_height_large
                        color: "transparent"
                        Image {
                            height: parent.height * 2/3
                            width: parent.height * 2/3
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            source: image_url
                        }
                    }

                    Column {
                        Text {
                            text: '<b>' + title + '</b>'
                            font.pointSize: app_settings.font_point_size
                            font.italic: false
                            color: "white"
                        }
                        Text {
                            text: description
                            font.pointSize: app_settings.font_point_size
                            font.italic: false
                            color: "white"
                        }
                    }
                }
            }
        }

        ListView {
            // Dimensions.
            height: app_settings.page_height
            width: app_settings.page_width

            // Positioning.
            x: app_settings.page_x
            y: 0
            z: app_settings.page_z

            model: TestModel {}
            header: list_view_header
            headerPositioning : ListView.PullBackHeader
            delegate: list_view_delegate
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            focus: true
        }

    }

}

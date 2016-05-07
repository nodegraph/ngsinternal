import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import octoplier.config 1.0
import octoplier.menus 1.0
import octoplier.pages 1.0
import octoplier.tools 1.0
import octoplier.data 1.0


Rectangle {
    id: menu_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    color: app_settings.ng_bg_color

    property var model:  NodeContextChoices {}

    Component {
        id: list_view_delegate
        Rectangle {
            id: delegate
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
                    anchors.verticalCenter: parent.verticalCenter
                    Text {
                        text: '<b>' + title + '</b>'
                        font.pointSize: app_settings.menu_page_title_point_size
                        font.italic: false
                        color: "white"
                    }
                    Text {
                        text: description
                        font.pointSize: app_settings.menu_page_description_point_size
                        font.italic: false
                        color: "white"
                        width: app_settings.page_width - app_settings.list_item_height_large - app_settings.action_bar_right_margin
                        wrapMode: Text.WordWrap
                    }
                }
            }
            MouseArea {
                id: mousearea1
                anchors.fill: parent
                onPressed: {
                    mouse.accepted = true
                    console.debug("rect click")
                    delegate.ListView.view.currentIndex = index
                    if (typeof next_model !== "undefined") {
                        var next_menu_model = app_loader.load_component(next_model, app_window, {})
                        var next_page = app_loader.load_component("qrc:///qml/octoplier/menus/MenuPage.qml", this, {})
                        next_page.model = next_menu_model
                        stack_view.push(next_page);
                        stack_view_header.push_title(next_menu_model.get(0).navigator_title)
                        //stack_view.push({item: "qrc:///qml/octoplier/menus/MenuPage.qml", properties: {model: next_model}})
                    } else if (typeof script != "undefined") {
                        eval(script)
                    }
                }
            }
        }
    }

    ListView {
        // Dimensions.
        height: app_settings.menu_page_height
        width: app_settings.menu_page_width

        // Positioning.
        x: 0
        y: 0
        z: 0

        // CurrentIndex.
        currentIndex: -1

        //NodeContextChoices {}
        model: menu_page.model

        delegate: list_view_delegate
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        highlightMoveDuration: 0
        focus: true
    }

}

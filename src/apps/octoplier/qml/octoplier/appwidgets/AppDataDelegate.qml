import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import octoplier.appconfig 1.0
import octoplier.stackedpages 1.0
import octoplier.pages 1.0
import octoplier.appwidgets 1.0
import octoplier.menumodels 1.0

Rectangle {
    id: data_delegate
    width: parent.width
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
                text: title
                font.family: "Arial"
                font.bold: true
                font.pointSize: app_settings.menu_page_title_point_size
                font.italic: false
                color: "white"
            }
            Text {
                text: description
                font.family: "Arial"
                font.pointSize: app_settings.menu_page_description_point_size
                font.italic: false
                color: "white"
                width: app_settings.page_width - app_settings.list_item_height_large - app_settings.action_bar_right_margin
                wrapMode: Text.WordWrap
            }
        }
    }
    MouseArea {
        id: mouse_area
        anchors.fill: parent
        onPressed: {
            mouse.accepted = true
            data_delegate.ListView.view.currentIndex = index
        }

        onDoubleClicked: {
            console.log("on double clicked!")
            mouse.accepted = true
            data_delegate.ListView.view.currentIndex = index
           
            if (typeof child_object_data !== "undefined") {
                data_delegate.ListView.view.parent_stack_view.view_object(title, child_object_data);
            } else if (typeof child_array_data !== "undefined") {
                console.log("on clicked array: " + Array.isArray(child_array_data))
                console.log("type of array: " + (typeof child_array_data))
                data_delegate.ListView.view.parent_stack_view.view_array(title, child_array_data);
            } else if (typeof script != "undefined") {
                data_delegate.ListView.view.parent_stack_view.execute_script(script)
            }
        }
    }
}

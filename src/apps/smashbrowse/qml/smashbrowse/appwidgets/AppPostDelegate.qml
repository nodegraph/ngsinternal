import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import smashbrowse.appconfig 1.0
import smashbrowse.stackedpages 1.0
import smashbrowse.pages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.menumodels 1.0


Rectangle {
    id: post_delegate
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
                source: "qrc:///icons/ic_insert_drive_file_white_48dp.png"
            }
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            Text {
                text: title
                font.bold: true
                font.family: "Arial"
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
                wrapMode: Text.WordWrap
            }
        }
    }
    MouseArea {
        id: mouse_area
        anchors.fill: parent
        onPressed: {
            mouse.accepted = true
            post_delegate.ListView.view.currentIndex = index
        }
        onDoubleClicked: {
            mouse.accepted = true
            post_delegate.ListView.view.currentIndex = index
        }
    }
}

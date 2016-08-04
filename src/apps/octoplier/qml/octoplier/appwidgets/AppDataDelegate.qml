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
            mouse.accepted = true
            // Set current index.
            data_delegate.ListView.view.currentIndex = index
            // Use functionality implemented on our containing stack view.
            var stack_view = data_delegate.ListView.view.parent_stack_view
            // Take action.
            if (typeof our_object_value !== "undefined") {
                stack_view.view_object(title, our_object_value);
            } else if (typeof our_array_value !== "undefined") {
                stack_view.view_array(title, our_array_value);
            } else if (typeof our_script != "undefined") {
                stack_view.execute_script(our_script)
            } else if (stack_view.allow_editing) {
                if (typeof our_string_value != "undefined") {
                    var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/EditStringPage.qml", edit_node_page, {})
                    page.init(our_string_value)
                    stack_view.push_page(title, page)
                } else if (typeof our_boolean_value != "undefined") {
                    var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/EditBooleanPage.qml", edit_node_page, {})
                    page.init(title, our_boolean_value)
                    stack_view.push_page(title, page)
                } else if (typeof our_number_value != "undefined") {
                    var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/EditNumberPage.qml", edit_node_page, {})
                    page.init(title, our_number_value)
                    stack_view.push_page(title, page)
                } else if (typeof our_enum_value != "undefined") {
                    var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/EditEnumPage.qml", edit_node_page, {})
                    page.init(title, our_enum_value)
                    stack_view.push_page(title, page)
                }
            }
        }
    }
}

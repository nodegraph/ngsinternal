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

    function get_image_url() {
        var stack_view = data_delegate.ListView.view.parent_stack_view
        var path = stack_view.get_title_path(1, depth_index+1)
        path.push(data_name)
        return stack_view.get_image_url(path)
    }

    function get_value_as_string() {
        var stack_view = data_delegate.ListView.view.parent_stack_view
        var path = stack_view.get_title_path(1, depth_index+1)
        path.push(data_name)
        return stack_view.get_value_as_string(path)
    }

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
                source: data_delegate.ListView.view.parent_stack_view ? get_image_url() : ""
            }
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            Text {
                text: data_name
                font.family: "Arial"
                font.bold: true
                font.pointSize: app_settings.menu_page_title_point_size
                font.italic: false
                color: "white"
            }
            Text {
                text: data_delegate.ListView.view.parent_stack_view ? get_value_as_string() : ""
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

            // Get our current value and type.
            var path = stack_view.get_title_path(1, depth_index+1)
            path.push(data_name)
            var value = stack_view.get_value(path)
            var value_type = stack_view.get_value_type(path)

            // Push a different page depending on the value type.
            switch(value_type) {
            case 'string_type':
                if (stack_view.allow_editing) {
                    var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/EditStringPage.qml", edit_node_page, {})
                    page.init(value)
                    page.set_title(data_name)
                    stack_view.push_page(page)
                }
                break
            case 'boolean_type':
                if (stack_view.allow_editing) {
                    var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/EditBooleanPage.qml", edit_node_page, {})
                    page.init(data_name, our_boolean_value)
                    page.set_title(data_name)
                    stack_view.push_page(page)
                }
                break
            case 'number_type':
                if (stack_view.allow_editing) {
                    var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/EditNumberPage.qml", edit_node_page, {})
                    page.init(data_name, our_number_value)
                    page.set_title(data_name)
                    stack_view.push_page(page)
                }
                break
            case 'dictionary_type':
                stack_view.view_object(data_name, value);
                break
            case 'array_type':
                stack_view.view_array(data_name, value);
                break
            default:
                console.log("Error: AppDataDelegate::onDoubleClicked encountered unknown type: " + value_type)
                break
            }
        }
    }
}

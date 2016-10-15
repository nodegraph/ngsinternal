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
    id: data_delegate
    width: parent.width
    height: app_settings.list_item_height_large
    border.color: "white"
    border.width: app_settings.list_item_border_width
    color: "transparent"

    // Note: the "name" and "value" variables are not declared here.
    // They come from the model and are exposed here by QML.

    function get_stack_view() {
        return data_delegate.ListView.view._list_page.Stack.view
    }

    function get_stack_page() {
        return data_delegate.ListView.view._list_page.Stack.view._stack_page
    }

    function get_image_for_value() {
        var path = get_stack_view().get_title_path(1, depth_index+1)
        path.push(name)
        var value = get_stack_page().get_value(path)
        return get_stack_page().get_image_for_value(value)
    }

    function get_string_for_value() {
        var path = get_stack_view().get_title_path(1, depth_index+1)
        path.push(name)
        var value = get_stack_page().get_value(path)
        return get_stack_page().get_string_for_value(value)
    }

    Row {
        // The large icon on the left that indicates the data type.
        Rectangle {
            height: app_settings.list_item_height_large
            width: app_settings.list_item_height_large
            color: "transparent"
            Image {
                height: parent.height * 2/3
                width: parent.height * 2/3
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                source: get_stack_view() ? get_image_for_value() : ""
            }
        }

        // The textual content on the right that refers to the data value.
        Column {
            anchors.verticalCenter: parent.verticalCenter
            // The name of the data in large text on top.
            Text {
                text: name
                font.family: "Arial"
                font.bold: true
                font.pointSize: app_settings.menu_page_title_point_size
                font.italic: false
                color: "white"
            }
            // The value of the data in smaller text on bottom.
            Text {
                text: get_stack_view() ? get_string_for_value() : ""
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
            // We need to specify the data_delegate id, in order to get the right ListView.
            data_delegate.ListView.view.currentIndex = index
        }

        onDoubleClicked: {
            mouse.accepted = true

            // Set current index.
            // We need to specify the data_delegate id, in order to get the right ListView.
            data_delegate.ListView.view.currentIndex = index

            // Use functionality implemented on our containing stack view.
            var stack_view = get_stack_view()
            var stack_page = get_stack_page()

            // Get our current value and type.
            var path = stack_view.get_title_path(1, depth_index+1)
            path.push(name)
            var value = stack_page.get_value(path)
            var value_type = app_enums.get_js_enum(value)

            // Push a different page depending on the value type.
            switch(value_type) {
            case js_enum.kString:
                if (stack_page._allow_edits) {
                    var page = app_loader.load_component("qrc:///qml/smashbrowse/stackedpages/EditStringPage.qml", edit_node_page, {})
                    page.set_value(value)
                    page.set_title(name)
                    stack_view.push_page(page)
                }
                break
            case js_enum.kBoolean:
                if (stack_page._allow_edits) {
                    var page = app_loader.load_component("qrc:///qml/smashbrowse/stackedpages/EditBooleanPage.qml", edit_node_page, {})
                    page.set_value(value)
                    page.set_title(name)
                    stack_view.push_page(page)
                }
                break
            case js_enum.kNumber:
                if (stack_page._allow_edits) {
                    var page = app_loader.load_component("qrc:///qml/smashbrowse/stackedpages/EditNumberPage.qml", edit_node_page, {})
                    page.set_value(value)
                    page.set_title(name)
                    stack_view.push_page(page)
                }
                break
            case js_enum.kObject:
                stack_page.view_object(name, value);
                break
            case js_enum.kArray:
                stack_page.view_array(name, value);
                break
            default:
                console.log("Error: AppDataDelegate::onDoubleClicked encountered unknown type: " + value_type)
                break
            }
        }
    }
}

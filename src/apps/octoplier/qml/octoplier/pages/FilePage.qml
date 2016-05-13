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
import octoplier.data 1.0

Rectangle{
    id: file_page

    height: app_settings.page_height
    width: app_settings.page_width

    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    color: app_settings.ng_bg_color

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.file_mode) {
            list_view.currentIndex = file_model.get_working_row();
            visible = true;
        } else {
            visible = false;
        }
    }

    function on_open_more_options() {
        if (visible) {
            // The node graph menu is being launched from the action bar,
            // so we center any nodes that are created.
            //popup_menu_with_centering()
            menu_stack_page.center_new_nodes = true
            menu_stack_page.visible = true
            menu_stack_page.stack_view.clear()
            menu_stack_page.stack_view.push_model_name("FileActions")
        }
    }

    function get_current_title() {
        return file_model.get_title(list_view.currentIndex)
    }

    function get_current_description() {
        return file_model.get_description(list_view.currentIndex)
    }

    function update_current_graph(title, description) {
        file_model.update_graph(list_view.currentIndex, title, description)
    }

    function load_current() {
        file_model.load_graph(list_view.currentIndex)
        node_graph_page.node_graph.update()
        main_bar.on_switch_to_mode(app_settings.node_graph_mode)
    }

    function delete_current() {
        if (file_model.count == 1) {
            return
        }
        file_model.destroy_graph(list_view.currentIndex)
        node_graph_page.node_graph.update()
        main_bar.on_switch_to_mode(app_settings.node_graph_mode)
    }

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
                        source: "qrc:///icons/ic_insert_drive_file_white_48dp.png"
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
                    delegate.ListView.view.currentIndex = index
                }
                onDoubleClicked: {
                    mouse.accepted = true
                    console.debug("rect click")
                    delegate.ListView.view.currentIndex = index
                    file_model.load_graph(index)
                    node_graph_page.node_graph.update()
                    main_bar.on_switch_to_mode(app_settings.node_graph_mode)
                }
            }
        }
    }

    ListView {
        id: list_view
        // Dimensions.
        height: app_settings.menu_page_height
        width: app_settings.menu_page_width

        // Positioning.
        x: 0
        y: 0
        z: 0

        // CurrentIndex.
        currentIndex: -1

        model: file_model

        delegate: list_view_delegate
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        highlightMoveDuration: 0
        focus: true
    }

}





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
import octoplier.subpages 1.0

Rectangle{
    id: menu_stack_page

    height: app_settings.page_height
    width: app_settings.page_width

    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    color: app_settings.menu_stack_bg_color

    property bool center_new_nodes: false
    property var last_mode;

    property alias stack_view: stack_view
    property alias stack_view_header: stack_view_header

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.settings_mode) {
            visible = true;
        } else {
            visible = false;
        }
    }

    // Slots.
    function on_node_context_menu() {
        app_settings.vibrate()
        center_new_nodes = false
        stack_view.clear_models()
        stack_view.push_model_name("NodeActions")
        last_mode = app_settings.node_graph_mode
        visible = true
    }

    function on_group_node_context_menu(){
        app_settings.vibrate()
        center_new_nodes = false
        stack_view.clear_models()
        stack_view.push_model_name("GroupNodeActions")
        last_mode = app_settings.node_graph_mode
        visible = true
    }

    function on_node_graph_context_menu() {
        app_settings.vibrate()
        center_new_nodes = false
        stack_view.clear_models()
        stack_view.push_model_name("NodeGraphActions")
        last_mode = app_settings.node_graph_mode
        visible = true
    }

    function on_create_file_page() {
        create_file_page.visible = true
        create_file_page.mode = "create"
        stack_view.push(create_file_page)
        stack_view_header.push_header_title("Create File")
        last_mode = app_settings.file_mode
        visible = true
    }
    
    function on_edit_file_page() {
        create_file_page.visible = true
        create_file_page.mode = "update"
        create_file_page.title_field.text = file_page.get_current_title()
        create_file_page.description_field.text = file_page.get_current_description()
        stack_view.push(create_file_page)
        stack_view_header.push_header_title("Update File")
        last_mode = app_settings.file_mode
        visible = true
    }

    function on_url_entry_page() {
        if (web_browser_page.web_view_alias.loading) {
            web_browser_page.web_view_alias.stop()
        }

        url_entry_page.visible = true
        stack_view.push(url_entry_page)
        stack_view_header.push_header_title("Go to URL")
        last_mode = null
        visible = true
    }

    // The Stack View Header
    Rectangle {
        id: stack_view_header
        height: app_settings.action_bar_height
        width: app_settings.page_width

        x: 0
        y: 0
        z: 0

        color: app_settings.menu_stack_header_bg_color
        border.width: app_settings.menu_stack_header_border_width
        border.color: app_settings.menu_stack_header_border_color
        radius: app_settings.menu_stack_header_radius

        property var header_title_stack: []

        function push_header_title(title) {
            header_title_stack.push(title)
            title_text.text = header_title_stack[header_title_stack.length-1];
        }

        function pop_header_title() {
            header_title_stack.pop()
            title_text.text = header_title_stack[header_title_stack.length-1];
        }

        function clear_header_titles() {
            header_title_stack = []
        }

        AppImageButton {
            id: back_button
            anchors.verticalCenter: parent.verticalCenter
            x: app_settings.action_bar_left_margin

            tooltip_text: "Back up"
            image_url: "qrc:///icons/ic_arrow_back_white_48dp.png"
            //opacity: stack_view.depth > 1 ? 1 : 0
            onClicked: {
                if (stack_view.depth > 1) {
                    stack_view.pop_model()
                } else {
                    menu_stack_page.visible = false
                    if (last_mode) {
                    	main_bar.on_switch_to_mode(last_mode)
                    }
                }
            }
        }

        Text {
            id: title_text
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            text: "Value Operators"
            font.pointSize: app_settings.font_point_size
            font.italic: false
            color: "white"
        }

    }

    // The Scroll View.
    ScrollView {
        id: scroll_view

        // Dimensions.
        height: app_settings.page_height - app_settings.action_bar_height
        width: app_settings.page_width

        // Positioning.
        x: 0
        y: app_settings.action_bar_height
        z: 0

        // Scrolling Policy.
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        verticalScrollBarPolicy: (Qt.platform.os == "android") || (Qt.platform.os == "ios") ? Qt.ScrollBarAlwaysOff : Qt.ScrollBarAsNeeded

        // The Stack View.
        StackView {
            id: stack_view
            anchors.fill: parent
            // Implements back key navigation
            focus: true
            Keys.onReleased: if (event.key === Qt.Key_Back && stack_view.depth > 1) {
                                 stack_view.pop();
                                 event.accepted = true;
                             }

            delegate: StackViewDelegate {
                function transitionFinished(properties)
                {
                    properties.exitItem.opacity = 1
                }

                pushTransition: StackViewTransition {
                    PropertyAnimation {
                        target: enterItem
                        property: "opacity"
                        from: 0
                        to: 1
                    }
                    PropertyAnimation {
                        target: exitItem
                        property: "opacity"
                        from: 1
                        to: 0
                    }
                }
            }

            function push_model(next_model) {
                var next_page = app_loader.load_component("qrc:///qml/octoplier/menus/MenuPage.qml", app_window, {})
                next_page.model = next_model
                push(next_page);
                stack_view_header.push_header_title(next_model.get(0).navigator_title)
            }

            function push_model_url(url) {
                var next_menu_model = app_loader.load_component(url, this, {})
                var props = {}
                props["links_are_locked"]=app_window.node_graph_page.node_graph.links_are_locked()
                next_menu_model.update(props)
                push_model(next_menu_model)
            }

            function push_model_name(model_name) {
                var url = "qrc:///qml/octoplier/data/" + model_name + ".qml"
                push_model_url(url)
            }

            function pop_model() {
                pop()
                stack_view_header.pop_header_title()
            }

            function clear_models(){
                clear()
                stack_view_header.clear_header_titles()
            }

            function execute_script(script) {
                eval(script)
            }

        }



    }

}

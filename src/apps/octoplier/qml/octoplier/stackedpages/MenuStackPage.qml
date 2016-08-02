import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import octoplier.appconfig 1.0
import octoplier.stackedpages 1.0
import octoplier.appwidgets 1.0
import octoplier.menumodels 1.0


// The menu_stack_page holds a header and a scrollable StackView.
// The StackView can hold MenuPages.
// Each MenuPage can hold a MenuModel.
// The stack view is filled dynamically at runtime according to the user actions.
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

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        stack_page: menu_stack_page
    }


    // The scroll view.
    AppScrollView {
        id: scroll_view

        // The main stack view.
        AppStackView{
            id: stack_view
            stack_view_header: stack_view_header

            function push_model(next_model) {
                var next_page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/MenuPage.qml", app_window, {})
                next_page.model = next_model
                next_page.title = next_model.title
                stack_view.push_page(next_page)
            }

            function push_model_url(url) {
                var next_menu_model = app_loader.load_component(url, this, {})
                var props = {}
                props["links_are_locked"]=app_window.node_graph_page.node_graph.links_are_locked()
                next_menu_model.update(props)
                push_model(next_menu_model)
            }

            function push_model_name(model_name) {
                var url = "qrc:///qml/octoplier/menumodels/" + model_name + ".qml"
                push_model_url(url)
            }
        }
    }
}

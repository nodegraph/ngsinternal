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

    // Appearance.
    color: app_settings.menu_stack_bg_color

    // Properties.
    property bool center_new_nodes: false

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
        stack_view.clear_pages()
        stack_view.push_by_names("Node Options", "MenuPage", "NodeActions")
        console.log('stack view depth: ' + stack_view.depth)
        visible = true
    }

    function on_group_node_context_menu(){
        app_settings.vibrate()
        center_new_nodes = false
        stack_view.clear_pages()
        stack_view.push_by_names("Group Node Options", "MenuPage", "GroupNodeActions")
        visible = true
    }

    function on_node_graph_context_menu() {
        app_settings.vibrate()
        center_new_nodes = false
        stack_view.clear_pages()
        stack_view.push_by_names("Node Graph Options", "MenuPage", "NodeGraphActions")
        visible = true
    }

    function show_options(model_name) {
        center_new_nodes = true
        visible = true
        stack_view.clear_pages()
        stack_view.push_by_names("Options", "MenuPage", model_name)
    }

    // The main stack view.
    AppStackView{
        id: stack_view
    }
}

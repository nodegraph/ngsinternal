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


AppStackPage{
    id: node_action_stack_page

    // Our Properties.
    property bool center_new_nodes: false

    // Framework Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.settings_mode) {
            visible = true;
        } else {
            visible = false;
        }
    }

    // -------------------------------------------------------------------------------------------
    // Create/Remove Pages.
    // -------------------------------------------------------------------------------------------

    function on_node_context_menu() {
        app_settings.vibrate()
        center_new_nodes = false
        stack_view.clear_pages()
        stack_view.push_by_names("Node Options", "NodeActionPage", "NodeActions")
        visible = true
    }

    function on_group_node_context_menu(){
        app_settings.vibrate()
        center_new_nodes = false
        stack_view.clear_pages()
        stack_view.push_by_names("Group Node Options", "NodeActionPage", "GroupNodeActions")
        visible = true
    }

    function on_node_graph_context_menu() {
        app_settings.vibrate()
        center_new_nodes = false
        stack_view.clear_pages()
        stack_view.push_by_names("Node Graph Options", "NodeActionPage", "NodeGraphActions")
        visible = true
    }

    function show_options(model_name) {
        center_new_nodes = true
        visible = true
        stack_view.clear_pages()
        stack_view.push_by_names("Options", "NodeActionPage", model_name)
    }

}

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import smashbrowse.appconfig 1.0
import smashbrowse.stackedmodepages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.contentpages.listmodels 1.0


BaseStackPage{
    id: page

    // Our Properties.
    property bool center_new_nodes: false

    // Framework Methods.
    function on_switch_to_mode(mode) {
    	// We always hide ourself, as there isn't actually a mode for us.
        visible = false;
    }

    // -------------------------------------------------------------------------------------------
    // Create/Remove Pages.
    // -------------------------------------------------------------------------------------------

    function on_node_context_menu() {
        app_settings.vibrate()
        center_new_nodes = false
        visible = true
        if (!show_busy_page()) {
        	stack_view.clear_pages()
        	stack_view.push_by_names("Node Options", "NGMenuListPage", "NodeActions")
        }
    }

    function on_group_node_context_menu(){
        app_settings.vibrate()
        center_new_nodes = false
       	visible = true
        if (!show_busy_page()) {
        	stack_view.clear_pages()
            stack_view.push_by_names("Group Node Options", "NGMenuListPage", "GroupNodeActions")
        }
    }

    function on_node_graph_context_menu() {
        app_settings.vibrate()
        center_new_nodes = false
        visible = true
        if (!show_busy_page()) {
        	stack_view.clear_pages()
            stack_view.push_by_names("Node Graph Options", "NGMenuListPage", "NodeGraphActions")
        }
    }

    function show_options(model_name) {
		app_settings.vibrate()
		center_new_nodes = true
        visible = true
        if (!show_busy_page()) {
        	stack_view.clear_pages()
            stack_view.push_by_names("Options", "NGMenuListPage", model_name)
        }
    }

}

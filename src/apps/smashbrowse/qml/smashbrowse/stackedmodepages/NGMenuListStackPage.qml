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

    function show_busy_page() {
        var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/warningpages/ProcessingPage.qml", ng_menu_list_stack_page, {})
        push_page.visible = true
        push_page.set_title("Processing Nodes")
        stack_view.push_page(push_page)
    }

    function on_node_context_menu() {
        app_settings.vibrate()
        center_new_nodes = false
        stack_view.clear_pages()
        if (web_worker.is_busy_cleaning()) {
            show_busy_page()
        } else {
        	stack_view.push_by_names("Node Options", "NGMenuListPage", "NodeActions")
        }
        visible = true
    }

    function on_group_node_context_menu(){
        app_settings.vibrate()
        center_new_nodes = false
        stack_view.clear_pages()
        if (web_worker.is_busy_cleaning()) {
            show_busy_page()
        } else {
            stack_view.push_by_names("Group Node Options", "NGMenuListPage", "GroupNodeActions")
        }
        visible = true
    }

    function on_node_graph_context_menu() {
        app_settings.vibrate()
        center_new_nodes = false
        stack_view.clear_pages()
        if (web_worker.is_busy_cleaning()) {
            show_busy_page()
        } else {
            stack_view.push_by_names("Node Graph Options", "NGMenuListPage", "NodeGraphActions")
        }
        visible = true
    }

    function show_options(model_name) {
        center_new_nodes = true
        visible = true
        stack_view.clear_pages()
        if (web_worker.is_busy_cleaning()) {
            show_busy_page()
        } else {
            stack_view.push_by_names("Options", "NGMenuListPage", model_name)
        }
    }

}

import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import youmacro.appconfig 1.0
import youmacro.stackedmodepages 1.0
import youmacro.appwidgets 1.0
import youmacro.contentpages.listmodels 1.0


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
        quick_view.vibrate(10)
        center_new_nodes = false
        visible = true
        if (!show_busy_page()) {
        	stack_view.clear_pages()
        	stack_view.push_by_names("manage node", "NGMenuListPage", "NodeActions")
        }
    }

    function on_group_node_context_menu(){
        quick_view.vibrate(10)
        center_new_nodes = false
       	visible = true
        if (!show_busy_page()) {
        	stack_view.clear_pages()
            stack_view.push_by_names("manage group node", "NGMenuListPage", "GroupNodeActions")
        }
    }

    function on_node_graph_context_menu() {
        quick_view.vibrate(10)
        center_new_nodes = false
        visible = true
        if (!show_busy_page()) {
        	stack_view.clear_pages()
            stack_view.push_by_names("manage node graph", "NGMenuListPage", "NodeGraphActions")
        }
    }

    function show_options(model_name) {
		quick_view.vibrate(10)
		center_new_nodes = true
        visible = true
        if (!show_busy_page()) {
        	stack_view.clear_pages()
            stack_view.push_by_names("more options", "NGMenuListPage", model_name)
        }
    }
    
    function rename_node() {
    	if (!node_graph_item.can_rename_node()) {
    		error_page.set_error_message("This node's name cannot be changed. The surrounding group looks for it under this name.")
    		main_bar.switch_to_current_mode()
    		error_page.show_page()
    		return
    	}
    	var push_page = app_loader.load_component("qrc:///qml/youmacro/contentpages/enterdatapages/EnterStringPage.qml", page, {})
        push_page.visible = true
        push_page.set_value("new_name")
        push_page.set_title("enter new node name")
        push_page.callback = function(name) {
    		node_graph_item.rename_node(name)
    		main_bar.switch_to_current_mode()
    	}
        stack_view.push_page(push_page)
    }

}

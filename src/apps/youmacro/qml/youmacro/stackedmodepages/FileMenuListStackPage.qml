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

import GUITypes 1.0

BaseStackPage{
    id: page

    // Framework Methods.
    function on_switch_to_mode(mode) {
        if (mode == GUITypes.FileMode) {
            visible = true;
            stack_view.push_by_names("manage files", "FileMenuListPage", "FileMenuActions")
        } else {
            visible = false;
            stack_view.clear_pages()
        }
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Main methods.
    // --------------------------------------------------------------------------------------------------------------------

    function on_finished_with_menu() {
    	main_bar.switch_to_node_graph()
    }

    function on_file_single_clicked(row) {
        // do nothing
    }

    function on_file_double_clicked(row) {
    	// Reset the task queue.
    	manipulator.force_stack_reset()
    	
    	// Make sure the browser is closed.
		task_queuer.close_browser()
		
        // Load the graph.
        file_model.load_graph(row)
        // Frame the graph.
        node_graph_item.frame_all()
        node_graph_item.update()
        // Finished.
        on_finished_with_menu()
    }
    
    function on_save_current() {
    	node_graph_item.save() 
    	on_finished_with_menu()
    }
    
    function on_publish_public_macro() {
    	var push_page = app_loader.load_component("qrc:///qml/youmacro/contentpages/enterdatapages/PublishMacroPage.qml", page, {})
        push_page.visible = true
        push_page.private_macro = false
        push_page.set_title("public macro details")
        stack_view.push_page(push_page)
    }
    
    function on_publish_private_macro() {
    	var push_page = app_loader.load_component("qrc:///qml/youmacro/contentpages/enterdatapages/PublishMacroPage.qml", page, {})
        push_page.visible = true
        push_page.private_macro = true
        push_page.set_title("private macro details")
        stack_view.push_page(push_page)
    }
    
    function on_import_public_macro() {
    	var push_page = app_loader.load_component("qrc:///qml/youmacro/contentpages/enterdatapages/SelectDropdownPage.qml", page, {})
        push_page.visible = true
        push_page.set_title("select a public macro")
        push_page.callback = function(option_text) {
        		if (option_text != "") {
        			ng_controller.create_public_macro_node(true, option_text)
        		}
        		main_bar.switch_to_node_graph()
        	}
        var macro_names = file_model.get_public_macro_names()
        push_page.set_option_texts(macro_names)
        stack_view.push_page(push_page)
        visible = true
    }
    
    function on_import_private_macro() {
    	var push_page = app_loader.load_component("qrc:///qml/youmacro/contentpages/enterdatapages/SelectDropdownPage.qml", page, {})
        push_page.visible = true
        push_page.set_title("select a private macro")
        push_page.callback = function(option_text) {
        		if (option_text != "") {
        			ng_controller.create_private_macro_node(true, option_text)
        		}
        		main_bar.switch_to_node_graph()
        	}
        var macro_names = file_model.get_private_macro_names()
        push_page.set_option_texts(macro_names)
        stack_view.push_page(push_page)
        visible = true
    }
    
    function on_import_app_macro() {
    	var push_page = app_loader.load_component("qrc:///qml/youmacro/contentpages/enterdatapages/SelectDropdownPage.qml", page, {})
        push_page.visible = true
        push_page.set_title("select an app macro")
        push_page.callback = function(option_text) {
        		if (option_text != "") {
        			ng_controller.create_app_macro_node(true, option_text)
        		}
        		main_bar.switch_to_node_graph()
        	}
        var macro_names = file_model.get_app_macro_names()
        push_page.set_option_texts(macro_names)
        stack_view.push_page(push_page)
        visible = true
    }
    
    function on_destroy_current(){
    	// Make sure the browser is closed.
		task_queuer.close_browser()
		
        // Destroy the graph.
        node_graph_item.destroy_graph()
        // Finished.
        on_finished_with_menu()
    }

	function on_create_graph_info(info) {
		// Make sure the browser is closed.
		task_queuer.close_browser()
		
		file_model.create_graph(info)
		node_graph_item.update()
		on_finished_with_menu()
	}
	
    function on_update_current_graph_info(info) {
        file_model.update_current_graph(info)
		file_model.sort_files()
		on_finished_with_menu()
	}

    function on_open_file_list() {
        var push_page = app_loader.load_component("qrc:///qml/youmacro/contentpages/listpages/FileListPage.qml", page, {})
        push_page.init()
        push_page.visible = true
        stack_view.push_page(push_page)
    }

    function on_create_file_info() {
        var push_page = app_loader.load_component("qrc:///qml/youmacro/contentpages/enterdatapages/CreateFilePage.qml", page, {})
        push_page.visible = true
        push_page.create_file = true
        push_page.set_title("enter file details")
        stack_view.push_page(push_page)
    }

    function on_edit_file_info() {
        var push_page = app_loader.load_component("qrc:///qml/youmacro/contentpages/enterdatapages/CreateFilePage.qml", page, {})
        push_page.visible = true
        push_page.create_file = false
        push_page.set_title("edit file details")
        stack_view.push_page(push_page)
    }
    
}

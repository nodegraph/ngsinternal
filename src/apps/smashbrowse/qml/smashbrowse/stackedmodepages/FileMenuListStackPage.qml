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

    // Framework Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.file_mode) {
            visible = true;
            stack_view.push_by_names("File Menu", "FileMenuListPage", "FileMenuActions")
        } else {
            visible = false;
            stack_view.clear_pages()
        }
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Main methods.
    // --------------------------------------------------------------------------------------------------------------------

    function on_finished_with_menu() {
        main_bar.switch_to_mode(app_settings.node_graph_mode)
    }

    function on_file_single_clicked(row) {
        // do nothing
    }

    function on_file_double_clicked(row) {
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
    
    function on_publish_user_macro() {
    	var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/PublishMacroPage.qml", page, {})
        push_page.visible = true
        push_page.set_title("Publish a Macro")
        stack_view.push_page(push_page)
    }
    
    function on_import_user_macro() {
    	var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/SelectDropdownPage.qml", page, {})
        push_page.visible = true
        push_page.set_title("Import a User Macro")
        push_page.callback = function(option_text) {
        		if (option_text != "") {
        			ng_controller.create_user_macro_node(true, option_text)
        		}
        		main_bar.switch_to_current_mode()
        	}
        var macro_names = node_graph_item.get_user_macro_names()
        push_page.set_option_texts(macro_names)
        stack_view.push_page(push_page)
        visible = true
    }
    
    function on_import_app_macro() {
    	var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/SelectDropdownPage.qml", page, {})
        push_page.visible = true
        push_page.set_title("Import an App Macro")
        push_page.callback = function(option_text) {
        		if (option_text != "") {
        			ng_controller.create_app_macro_node(true, option_text)
        		}
        		main_bar.switch_to_current_mode()
        	}
        var macro_names = node_graph_item.get_app_macro_names()
        push_page.set_option_texts(macro_names)
        stack_view.push_page(push_page)
        visible = true
    }
    
    function on_destroy_current(){
        // Destroy the graph.
        file_model.destroy_graph()
        // Finished.
        on_finished_with_menu()
    }

	function on_create_graph_info(info) {
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
        var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/listpages/FileListPage.qml", page, {})
        push_page.init()
        push_page.visible = true
        stack_view.push_page(push_page)
    }

    function on_create_file_info() {
        var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/CreateFilePage.qml", page, {})
        push_page.visible = true
        push_page.create_file = true
        push_page.set_title("New File")
        stack_view.push_page(push_page)
    }

    function on_edit_file_info() {
        var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/CreateFilePage.qml", page, {})
        push_page.visible = true
        push_page.create_file = false
        push_page.set_title("File Info")
        stack_view.push_page(push_page)
    }
    
}

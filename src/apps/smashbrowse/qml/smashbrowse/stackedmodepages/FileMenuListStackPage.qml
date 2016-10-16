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

AppStackPage{
    id: file_menu_stack_page

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
        var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/listpages/FileListPage.qml", file_menu_stack_page, {})
        page.init()
        page.visible = true
        stack_view.push_page(page)
    }

    function on_create_file_info() {
        var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/CreateFilePage.qml", file_menu_stack_page, {})
        page.visible = true
        page.create_file = true
        page.set_title("New File")
        stack_view.push_page(page)
    }

    function on_edit_file_info() {
        var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/CreateFilePage.qml", file_menu_stack_page, {})
        page.visible = true
        page.create_file = false
        page.set_title("File Info")
        stack_view.push_page(page)
    }
    
}

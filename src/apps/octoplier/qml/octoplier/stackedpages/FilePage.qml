import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import octoplier.appconfig 1.0
import octoplier.stackedpages 1.0
import octoplier.pages 1.0
import octoplier.appwidgets 1.0
import octoplier.menumodels 1.0

AppListPage {
    id: file_page

    // Setttings.
    show_back_button: file_page.parent_stack_view && (file_page.parent_stack_view.depth > 1)
    model: file_model
    delegate: AppFileDelegate{}

    // Our File Methods.
    function init(mode) {
        list_view.currentIndex = file_model.get_working_row();
    }

    // --------------------------------------------------------------------------------------------
    // Info about the current file highlighted in the list view.
    // Note that the current file is not necessarily have it's node graph loaded.
    // --------------------------------------------------------------------------------------------

    function get_current_index() {
        return list_view.currentIndex
    }

    function get_current_title() {
        return file_model.get_title(get_current_index())
    }

    function get_current_description() {
        return file_model.get_description(get_current_index())
    }

    function get_current_auto_run() {
        return file_model.get_auto_run(get_current_index())
    }

    function get_current_auto_run_interval() {
        return file_model.get_auto_run_interval(get_current_index())
    }

    function get_current_hide_passwords() {
        return file_model.get_hide_passwords(get_current_index())
    }

    function get_current_lock_links() {
        return file_model.get_lock_links(get_current_index())
    }

    function get_current_max_node_posts() {
        return file_model.get_max_node_posts(get_current_index())
    }

    // --------------------------------------------------------------------------------------------
    // Operations on the current file highlighted in the list view.
    // --------------------------------------------------------------------------------------------

    function update_current_graph(file_info) {    
        file_model.set_title(list_view.currentIndex, file_info.title)
        file_model.set_description(list_view.currentIndex, file_info.description)
        file_model.set_auto_run(list_view.currentIndex, file_info.auto_run)
        file_model.set_auto_run_interval(list_view.currentIndex, file_info.auto_run_interval)
        file_model.set_hide_passwords(list_view.currentIndex, file_info.hide_passwords)
        file_model.set_lock_links(list_view.currentIndex, file_info.lock_links)
        file_model.set_max_node_posts(list_view.currentIndex, file_info.max_node_posts)
        file_model.sort_files()
    }

    function load_current() {
        node_graph_page.node_graph.surface_to_root()
        file_model.load_graph(list_view.currentIndex)
        node_graph_page.node_graph.frame_all()
        node_graph_page.node_graph.update()
        main_bar.on_switch_to_mode(app_settings.node_graph_mode)
    }

    function delete_current() {
        if (file_model.count == 1) {
            return
        }
        file_model.destroy_graph(list_view.currentIndex)
        node_graph_page.node_graph.update()
        main_bar.on_switch_to_mode(app_settings.node_graph_mode)
    }
}

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import smashbrowse.appconfig 1.0
import smashbrowse.stackedpages 1.0
import smashbrowse.pages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.menumodels 1.0

AppListPage {
    id: file_page

    // Setttings.
    show_back_button: file_page.parent_stack_view && (file_page.parent_stack_view.depth > 1)
    model: file_model
    model_is_dynamic: false
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

    function get_current_setting(setting) {
        return file_model.get_setting(get_current_index(), setting)
    }

    // --------------------------------------------------------------------------------------------
    // Operations on the current file highlighted in the list view.
    // --------------------------------------------------------------------------------------------

    function update_current_graph(settings) {
        file_model.update_graph(list_view.currentIndex, settings)
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

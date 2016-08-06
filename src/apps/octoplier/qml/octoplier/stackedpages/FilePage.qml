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

    function get_current_title() {
        return file_model.get_title(list_view.currentIndex)
    }

    function get_current_description() {
        return file_model.get_description(list_view.currentIndex)
    }

    function update_current_graph(title, description) {
        file_model.update_graph(list_view.currentIndex, title, description)
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

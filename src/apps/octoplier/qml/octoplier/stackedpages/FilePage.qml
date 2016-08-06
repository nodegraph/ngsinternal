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

// MenuPage's are created dynamically and given a MenuModel.
// They are then pushed on to the menu_stack_page.
Rectangle {
    id: file_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Appearance.
    color: app_settings.menu_page_bg_color

    // Our Model.
    property var model:  ListModel{}

    // Our Dependencies.
    property alias parent_stack_view: list_view.parent_stack_view //  This is set when pushed onto the stack view.

    // Our Methods.
    function set_title(title) {
        stack_view_header.title_text = title
    }
    function get_title() {
        return stack_view_header.title_text
    }

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

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        stack_view: parent_stack_view
        show_back_button: file_page.parent_stack_view && (file_page.parent_stack_view.depth > 1)
    }

    ListView {
        id: list_view
        clip: true
        
        // Dimensions.
        height: app_settings.menu_page_height
        width: app_settings.menu_page_width

        // Positioning.
        x: 0
        y: app_settings.action_bar_height
        z: 0

        // Our model and delegate.
        model: file_model
        delegate: AppFileDelegate{}
        currentIndex: -1

        // Appearance.
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        highlightMoveDuration: 0
        focus: true

        // Our parenting stack view.
        property var parent_stack_view: null
    }
}

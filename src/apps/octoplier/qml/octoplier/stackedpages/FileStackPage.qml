import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import octoplier.appconfig 1.0
import octoplier.stackedpages 1.0
import octoplier.appwidgets 1.0
import octoplier.menumodels 1.0

Rectangle{
    id: file_stack_page

    height: app_settings.page_height
    width: app_settings.page_width

    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    color: app_settings.action_bar_bg_color //app_settings.ng_bg_color

    // Internal Properties.
    property var initialized: false

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.file_mode) {
            visible = true;
            // Make sure we're initialized.
            init()
            // Reset the stack to only show the list of files.
            reset()
        } else {
            visible = false;
            reset()
        }
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Main methods.
    // --------------------------------------------------------------------------------------------------------------------

    function init() {
        // Return if we're already intialized.
        if (initialized) {
            return
        }
        // The first page is our list of files.
        // Subsequent pages will be options for the selected file.
        var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/FilePage.qml", menu_stack_page, {})
        page.visible = true
        page.set_title("All Files")
        stack_view.push_page(page)
        initialized = true
    }

    function reset() {
        stack_view.close_file_options()
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Methods which forward to the file page, which is the first page.
    // --------------------------------------------------------------------------------------------------------------------

    function get_file_page() {
        return stack_view.get(0)
    }

    function load_current() {
        get_file_page().load_current()
        reset()
    }

    function delete_current() {
        get_file_page().delete_current()
        reset()
    }

    function update_current_graph(title, description) {
        get_file_page().update_current_graph(title, description)
        reset()
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Add new pages onto the stack. Usually called by other actions.
    // --------------------------------------------------------------------------------------------------------------------

    function on_create_file_page() {
        var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/CreateFilePage.qml", menu_stack_page, {})
        page.visible = true
        page.mode = "create"
        page.set_title("Create File")
        stack_view.push_page(page)
        visible = true
    }

    function on_edit_file_page() {
        var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/CreateFilePage.qml", menu_stack_page, {})
        page.visible = true
        page.mode = "update"
        page.title_field.text = get_file_page().get_current_title()
        page.description_field.text = get_file_page().get_current_description()
        page.set_title("Edit File Info")
        stack_view.push_page(page)
        visible = true
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Our internal structure.
    // --------------------------------------------------------------------------------------------------------------------

    // The main stack view.
    AppStackView{
        id: stack_view

        function open_file_options() {
            stack_view.push_by_names(get_file_page().get_current_title(), "MenuPage", "FileActions")
        }

        function close_file_options() {
            // Pop everything except the file page.
            stack_view.pop(get_file_page())
        }
    }

}

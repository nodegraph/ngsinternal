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

AppStackPage{
    id: file_stack_page

    // Internal Properties.
    property var initialized: false

    // Framework Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.file_mode) {
            visible = true;
            // Make sure we're initialized.
            init()
            // Reset the stack to only show the list of files.
            on_close_file_options()
        } else {
            visible = false;
            on_close_file_options()
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

    // --------------------------------------------------------------------------------------------------------------------
    // Methods which forward to the file page, which is the first page.
    // --------------------------------------------------------------------------------------------------------------------

    function get_file_page() {
        return stack_view.get(0)
    }

    function load_current() {
        get_file_page().load_current()
        on_close_file_options()
    }

    function delete_current() {
        get_file_page().delete_current()
        on_close_file_options()
    }

    function update_current_graph(title, description) {
        get_file_page().update_current_graph(title, description)
        on_close_file_options()
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Create/Remove Pages. Usually called by other actions.
    // --------------------------------------------------------------------------------------------------------------------

    function on_open_file_options() {
        stack_view.push_by_names(get_file_page().get_current_title(), "MenuPage", "FileActions")
    }

    function on_close_file_options() {
        // Pop everything except the file page.
        stack_view.pop(get_file_page())
    }

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

}

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


	// The file page is always at the bottom of the stack.
    function get_file_page() {
        return stack_view.get(0)
    }

	// Push file options page onto the stack.
    function on_open_file_options() {
        stack_view.push_by_names(get_file_page().get_current_setting('title'), "MenuPage", "FileActions")
    }

	// Pop the file options off the stack.
    function on_close_file_options() {
        // Pop everything except the file page.
        stack_view.pop(get_file_page())
    }

	// Push the create file page onto the stack.
    function on_create_file_page() {
        var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/CreateFilePage.qml", menu_stack_page, {})
        page.visible = true
        page.create_file = true
        page.set_title("Create File")
        stack_view.push_page(page)
        visible = true
    }

	// Push the edit file page onto the stack.
    function on_edit_file_page() {
        var page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/CreateFilePage.qml", menu_stack_page, {})
        page.visible = true
        page.create_file = false
        page.title_field.text = get_file_page().get_current_setting('title')
        page.description_field.text = get_file_page().get_current_setting('description')
        page.set_title("Edit File Info")
        stack_view.push_page(page)
        visible = true
    }

}

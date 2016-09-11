import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import smashbrowse.appconfig 1.0
import smashbrowse.stackedpages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.menumodels 1.0



AppStackPage{
    id: web_action_stack_page

    // Framework Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.web_actions_mode) {
            visible = true;
        } else {
            visible = false;
        }
    }

    // -------------------------------------------------------------------------------------------
    // Create/Remove Pages.
    // -------------------------------------------------------------------------------------------

    function on_show_web_action_menu() {
        app_settings.vibrate()
        stack_view.clear_pages()
        stack_view.push_by_names("Web Actions", "WebActionPage", "WebActions")
        visible = true
    }

    function on_show_iframe_menu() {
        app_settings.vibrate()
        stack_view.clear_pages()
        stack_view.push_by_names("IFrame Switch", "WebActionPage", "IFrameActions")
        visible = true
    }

    function on_url_entry() {
        var page = app_loader.load_component("qrc:///qml/smashbrowse/stackedpages/EnterStringPage.qml", web_action_stack_page, {})
        page.visible = true
        page.init("www.")
        page.set_title("Enter URL")
        page.callback = app_comm.navigate_to.bind(app_comm)
        stack_view.push_page(page)
        visible = true
    }

    function on_type_text() {
        var page = app_loader.load_component("qrc:///qml/smashbrowse/stackedpages/EnterStringPage.qml", web_action_stack_page, {})
        page.visible = true
        page.init("")
        page.set_title("Type Text")
        page.callback = app_comm.type_text.bind(app_comm)
        stack_view.push_page(page)
        visible = true
    }

    function on_select_from_dropdown() {
        var page = app_loader.load_component("qrc:///qml/smashbrowse/stackedpages/SelectDropdownPage.qml", web_action_stack_page, {})
        page.visible = true
        page.init("")
        page.set_title("Select from Dropdown")
        page.callback = app_comm.select_from_dropdown.bind(app_comm)
        page.set_option_texts(app_comm.get_option_texts())
        stack_view.push_page(page)
        visible = true
    }

}

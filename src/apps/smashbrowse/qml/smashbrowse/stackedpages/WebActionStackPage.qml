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

    function on_show_web_action_menu(msg) {
        console.log("aml got msg: " + JSON.stringify(msg))
        app_settings.vibrate()
        stack_view.clear_pages()
        stack_view.push_by_names("Web Actions", "WebActionPage", "WebActions")
        visible = true
    }

    // Push the edit file page onto the stack.
    function on_url_entry() {
        var page = app_loader.load_component("qrc:///qml/smashbrowse/stackedpages/EnterURLPage.qml", web_action_stack_page, {})
        page.visible = true
        page.init("www.")
        page.set_title("Enter URL")
        stack_view.push_page(page)
        visible = true
    }

}

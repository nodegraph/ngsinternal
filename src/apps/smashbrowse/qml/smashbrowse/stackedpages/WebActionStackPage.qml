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

import Message 1.0



AppStackPage{
    id: web_action_stack_page

    property var msg: null

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
        web_action_stack_page.msg = msg
        console.log("got msg: " + JSON.stringify(msg))

        app_settings.vibrate()
        stack_view.clear_pages()
        stack_view.push_by_names("Web Actions", "WebActionPage", "WebActions")
        visible = true
    }

    function get_all_cookies() {
        console.log('get all cookies: ' + Message.KGetAllCookies)
        console.log('clear all cookies: ' + Message.KClearAllCookies)

        var req = app_comm.build_message(msg.iframe, Message.KGetAllCookies)
        app_comm.handle_request_from_app(req)
    }

    function clear_all_cookies() {
        var req = app_comm.build_message(msg.iframe, Message.KClearAllCookies)
        app_comm.handle_request_from_app(req);
    }

    function set_all_cookies() {
        var req = app_comm.build_message(msg.iframe, Message.KSetAllCookies)
        app_comm.handle_request_from_app(req);
    }

    function update_overlays() {
        var req = app_comm.build_message(msg.iframe, Message.KUpdateOverlays)
        app_comm.handle_request_from_app(req);
    }

}

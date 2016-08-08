import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtWebView 1.1

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.appwidgets 1.0

// If there are android specific layout issues in the future you can try the
// following trick. Use an "if" statement on the plaform type.
// y: Qt.platform.os === "android" ? app_settings.action_bar_height : 0

Rectangle {
    id: web_browser_page
    // Aliases to our internal objects.
    property alias web_view_alias: web_view;

    color: app_settings.menu_stack_bg_color // "#FF01579B" //app_settings.action_bar_bg_color // app_settings.menu_stack_bg_color

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width
    
    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    // Signals.
    signal popup_menu()
    signal switch_to_mode(int mode)
    
    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.web_browser_mode) {
            visible = true
            menu_stack_page.visible = false
        } else {
            if (visible) {
                hide_web_view()
            }
        }
    }
    function on_open_more_options() {
        if (visible) {
            // Hide this web browser page, as on ios the
            // webview don't allow anything to be drawn over them.
            hide_web_view()
			
			// Set up the menu stack page.            
            menu_stack_page.center_new_nodes = true
            menu_stack_page.visible = true
            menu_stack_page.stack_view.clear()
            menu_stack_page.stack_view.push_model_name("WebBrowserActions")
        }
    }

    function hide_web_view() {
        // This is a hack to fix a Qt5.6 issue where the android keyboard won't
        // popup anymore once it used in a field in the WebView.
        app_settings.dismiss_keyboard_from_webview()
        visible = false
        web_view.focus = false
        app_settings.dismiss_keyboard_from_webview()
    }

    function start_recording() {
        // Clear out our current event recording.
        web_view.events = []
        //web_view.events.push({type: 'goto_url', url: web_view.url})

        // Start recording.
        //web_view.recording = true

        app_comm.start_recording()
    }

    function stop_recording() {
        //web_view.recording = false
        app_comm.stop_recording()
        //app_comm.request_recording()
    }

    function replay() {
        app_comm.replay_last()
    }

    AppWebView {
        id: web_view
    }

}

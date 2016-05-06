import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4


Menu {
    id: web_browser_menu
    title: "Web Browser Actions"

    // Signals
    signal switch_to_mode(int mode)

    // Methods.
    function on_popup_menu() {
        app_settings.vibrate()
        popup()
    }

    // Dependencies.
    property var web_browser_page;

    // Init From Deps.
    function init_from_deps() {
        back.enabled = Qt.binding(function(){return web_browser_page.web_view_alias.canGoBack})
        forward.enabled = Qt.binding(function(){return web_browser_page.web_view_alias.canGoForward})
        stop.enabled = Qt.binding(function(){return web_browser_page.web_view_alias.loading})
    }

    MenuItem {
        text: "Go to Google"
        enabled: true
        onTriggered: {
            web_browser_page.web_view_alias.url = utils.url_from_input("www.google.com")
        }
    }
        
    MenuItem {
        text: "Go to URL"
        enabled: true
        onTriggered: {
            if (web_browser_page.web_view_alias.loading) {
                web_browser_page.web_view_alias.stop()
            }
            switch_to_mode(app_settings.url_entry_mode);
        }
    }
        
    MenuItem {
        id: back
        text: "Back"
        //enabled: web_browser_page.web_view_alias.canGoBack
        onTriggered: web_browser_page.web_view_alias.goBack()
    }

    MenuItem {
        id: forward
        text: "Forward"
        //enabled: web_browser_page.web_view_alias.canGoForward
        onTriggered: web_browser_page.web_view_alias.goForward()
    }

    MenuItem {
        id: refresh
        text: "Refresh"
        enabled: true
        onTriggered: {
            if (web_browser_page.web_view_alias.loading) {
                web_browser_page.web_view_alias.stop()
            }
            web_browser_page.web_view_alias.reload()
        }
    }

    MenuItem {
        id: stop
        text: "Stop"
        //enabled: web_browser_page.web_view_alias.loading
        onTriggered: web_browser_page.web_view_alias.stop()
    }

//    MenuItem {
//        text: "Go"
//        enabled: !web_browser_page.web_view_alias.loading
//        onTriggered: {
//            Qt.inputMethod.commit()
//            Qt.inputMethod.hide()
//            web_browser_page.web_view_alias.url = utils.url_from_input(urlField.text)
//        }
//    }
}

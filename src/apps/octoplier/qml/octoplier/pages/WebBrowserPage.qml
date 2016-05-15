import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtWebView 1.1

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.tools 1.0

// If there are android specific layout issues in the future you can try the
// following trick. Use an "if" statement on the plaform type.
// y: Qt.platform.os === "android" ? app_settings.action_bar_height : 0

Rectangle {

    // Aliases to our internal objects.
    property alias web_view_alias: web_view

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
            visible = true;
            menu_stack_page.visible = false
	   	} else {
            visible = false;
	    }
    }
    function on_open_more_options() {
        if (visible) {
            console.log("web browser is opening more options")
            
            // Hide this web browser page, as on mobile platforms the
            // webview don't allow anything to be drawn over them.
            visible = false
			
			// Set up the menu stack page.            
            menu_stack_page.last_mode = app_settings.web_browser_mode
            menu_stack_page.center_new_nodes = true
            menu_stack_page.visible = true
            menu_stack_page.stack_view.clear()
            menu_stack_page.stack_view.push_model_name("WebBrowserActions")
        } else {
            console.log("web browser is not opening more options")
        }
    }
    
    // -------------------------------------------------
    // Web View.   
    // -------------------------------------------------
    WebView {
        id: web_view
        objectName: "web_view_object"

        // Positioning.
        x: 0
        y: 0
        z: app_settings.page_z
        
        // Dimensions.
        height: parent.height
        width: parent.width

        // Initial url.
        url: "http://www.google.com"

        // Timer to pulse our web recorder.
//        Timer {
//            interval: 500
//            running: true
//            repeat: true
//            onTriggered: {
//                try {
//                    var eventRecorderScriptLoaded = function (result) {
//                        console.log("event recorder loaded")
//                    }

//                    var getEventHandler = function (result) {
//                        if (result === "event recorder not initialized") {
//                            console.log("installing events recorder")
//                            web_view.runJavaScript(
//                                        scripts.get_event_recorder_script(),
//                                        eventRecorderScriptLoaded)
//                        } else if (result != "null") {
//                            console.log("dequeuing:" + result)
//                        }
//                    }
//                    // Dequeue the next event info.
//                    web_view.runJavaScript(scripts.get_event_retriever_script(
//                                               ), getEventHandler)

//                    var getErrorsHandler = function (result) {
//                        if (result != "[]") {
//                            //console.log("debug messages:" + result)
//                        }
//                    }
//                    // Dequeue any debug messages that have built up.
//                    web_view.runJavaScript(scripts.get_error_retriever_script(
//                                               ), getErrorsHandler)
//                } catch (e) {
//                    console.log("Error in time triggered code: " + e)
//                }
//            }
//        }

        //                onLoadingChanged: {
        //                    if (loadRequest.status == WebView.LoadStartedStatus) {
        //                        //message_dialog.show(qsTr("page load has started"))
        //                    } else if (loadRequest.status == WebView.LoadStoppedStatus) {
        //                        message_dialog.show(qsTr("page load has stopped"))
        //                    } else if (loadRequest.status == WebView.LoadSucceededStatus) {
        //                        message_dialog.show(qsTr("page load has succeeded"))
        //                        // Inject the user blockedActions recorder.
        //                        web_view.runJavaScript(Scripts.OCTOPLIER_BROWSER_SCRIPTS.USER_ACTIONS_RECORDER_SCRIPT);
        //                    } else if (loadRequest.status == WebView.LoadFailedStatus) {
        //                        message_dialog.show(qsTr("page load has failed"))
        //                    } else {
        //                        message_dialog.show(qsTr("got an unknown load status"))
        //                    }
        //                }
    }
}

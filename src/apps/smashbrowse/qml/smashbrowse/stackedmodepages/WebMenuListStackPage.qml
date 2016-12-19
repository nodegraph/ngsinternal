import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import smashbrowse.appconfig 1.0
import smashbrowse.stackedmodepages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.contentpages.listmodels 1.0


BaseStackPage{
    id: page

    // Framework Methods.
    function on_switch_to_mode(mode) {
    	// We always hide ourself, as there isn't actually a mode for us.
        visible = false;
    }

    // -------------------------------------------------------------------------------------------
    // Create/Remove Pages.
    // -------------------------------------------------------------------------------------------

    function on_show_web_action_menu() {
        app_settings.vibrate()
        visible = true
        if (!show_busy_page()) {
	        stack_view.clear_pages()
	        stack_view.push_by_names("Web Actions", "WebMenuListPage", "WebActions")
     	}   
    }

    function on_url_entry() {
        var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/EnterStringPage.qml", page, {})
        push_page.visible = true
        push_page.set_value("www.")
        push_page.set_title("Enter URL")
        push_page.callback = function(url) {
        		web_recorder.record_navigate_to(url)
        		main_bar.switch_to_current_mode()
        	}
        stack_view.push_page(push_page)
        visible = true
    }

    function on_type_text() {
        var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/EnterStringPage.qml", page, {})
        push_page.visible = true
        push_page.set_value("")
        push_page.set_title("Type Text")
        push_page.callback = function(text) {
        		web_recorder.record_type_text(text)
        		main_bar.switch_to_current_mode()
        	}
        stack_view.push_page(push_page)
        visible = true
    }

    function on_select_from_dropdown() {
    	web_worker.queue_emit_option_texts()
    }
    
    // Receive option texts from the app worker.
    function on_select_option_texts(option_texts) {
    	console.log('got option texts: ' + option_texts)
    	var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/SelectDropdownPage.qml", page, {})
        push_page.visible = true
        push_page.set_title("Select from Dropdown")
        push_page.callback = function(option_text) {
        		web_recorder.record_select_from_dropdown(option_text)
        		main_bar.switch_to_current_mode()
        	}
        push_page.set_option_texts(option_texts)
        stack_view.push_page(push_page)
        visible = true
    }
}

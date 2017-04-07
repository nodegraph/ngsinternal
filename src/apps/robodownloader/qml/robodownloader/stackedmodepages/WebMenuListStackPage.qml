import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import robodownloader.appconfig 1.0
import robodownloader.stackedmodepages 1.0
import robodownloader.appwidgets 1.0
import robodownloader.contentpages.listmodels 1.0


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
        var push_page = app_loader.load_component("qrc:///qml/robodownloader/contentpages/enterdatapages/EnterStringPage.qml", page, {})
        push_page.visible = true
        push_page.set_value("www.")
        push_page.set_title("enter url")
        push_page.callback = function(url) {
        		browser_recorder.record_navigate_to(url)
        		main_bar.switch_to_current_mode()
        	}
        stack_view.push_page(push_page)
        visible = true
    }

    function on_type_text() {
        var push_page = app_loader.load_component("qrc:///qml/robodownloader/contentpages/enterdatapages/EnterStringPage.qml", page, {})
        push_page.visible = true
        push_page.set_value("")
        push_page.set_title("type text")
        push_page.callback = function(text) {
        		browser_recorder.record_type_text(text)
        		main_bar.switch_to_current_mode()
        	}
        stack_view.push_page(push_page)
        visible = true
    }
    
    function on_type_password() {
        var push_page = app_loader.load_component("qrc:///qml/robodownloader/contentpages/enterdatapages/EnterPasswordPage.qml", page, {})
        push_page.visible = true
        push_page.set_value("")
        push_page.set_title("type password")
        push_page.callback = function(text) {
        		browser_recorder.record_type_password(text)
        		main_bar.switch_to_current_mode()
        	}
        stack_view.push_page(push_page)
        visible = true
    }

    function on_choose_from_dropdown() {
    	var last_click_info = task_queuer.get_last_click_info()
    	var option_texts = last_click_info.option_texts
    	console.log('got option texts: ' + option_texts)
    	var push_page = app_loader.load_component("qrc:///qml/robodownloader/contentpages/enterdatapages/SelectDropdownPage.qml", page, {})
        push_page.visible = true
        push_page.set_title("select dropdown option")
        push_page.callback = function(option_text) {
        		browser_recorder.record_select_from_dropdown(option_text)
        		main_bar.switch_to_current_mode()
        	}
        push_page.set_option_texts(option_texts)
        stack_view.push_page(push_page)
        visible = true
    }
}

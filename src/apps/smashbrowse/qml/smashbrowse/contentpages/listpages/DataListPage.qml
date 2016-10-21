import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import smashbrowse.appconfig 1.0
import smashbrowse.stackedmodepages 1.0
import smashbrowse.fullpages 1.0
import smashbrowse.contentpages.listpages 1.0
import smashbrowse.contentpages.listdelegates 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.contentpages.listmodels 1.0

BaseListPage {
    id: page

    // Our settings.
    show_back_button: page.Stack.view && (page.Stack.view.depth > 1)
    delegate: DataListDelegate{}
    list_view.height: app_settings.menu_page_height - edit_bar.height
    
    property var resizable: false
    
    function get_stack_view() {
    	return Stack.view
    }
    
    function get_stack_page() {
    	return Stack.view._stack_page
    }
    
    function on_add() {
    	get_stack_page().on_add_element()
	}
	
	function on_edit() {
		var name = list_view.model.get(list_view.currentIndex).name
		get_stack_page().on_edit_element(name)
	}
	
	function on_remove() {
		var name = list_view.model.get(list_view.currentIndex).name
		get_stack_page().on_remove_element(name)
	}
	
	// Buttons.
    RowLayout {
    	id: edit_bar
    	Layout.maximumWidth: list_view.width
    	
    	anchors {
            left: list_view.left
            right: list_view.right
            bottom: parent.bottom
        }

        Item {Layout.fillWidth: true}
        AppLabelButton {
            text: "add"
            visible: page.resizable
            onClicked: {
                var path = page.Stack.view.get_title_path(1, page.Stack.view.depth)
                on_add(path)
            }
        }
        Rectangle {
            color: "transparent"
            height: app_settings.action_bar_height
            width: app_settings.button_spacing
        }
        AppLabelButton {
            text: "edit"
            onClicked: {
                on_edit()
            }
        }
        Rectangle {
            color: "transparent"
            height: app_settings.action_bar_height
            width: app_settings.button_spacing
        }
        AppLabelButton {
            text: "remove"
            visible: page.resizable
            onClicked: {
                var path = page.Stack.view.get_title_path(1, page.Stack.view.depth)
                on_remove(path)
            }
        }
        Item {Layout.fillWidth: true}
    }
}

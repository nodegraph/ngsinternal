import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import youmacro.appconfig 1.0
import youmacro.stackedmodepages 1.0
import youmacro.fullpages 1.0
import youmacro.contentpages.listpages 1.0
import youmacro.contentpages.listdelegates 1.0
import youmacro.appwidgets 1.0
import youmacro.contentpages.listmodels 1.0

BaseListPage {
    id: page

    // Setttings.
    model: file_model
    model_is_dynamic: false
    delegate: MacroListDelegate{}
    
    function get_stack_view() {
    	return Stack.view
    }
    
    function get_stack_page() {
    	return Stack.view._stack_page
    }

    // Our File Methods.
    function init() {
    	set_title("macros")
        list_view.currentIndex = file_model.get_working_row();
    }
    
    function on_add() {
    	get_stack_page().on_show_macro_list()
	}
	
	function on_edit() {
		get_stack_page().on_edit_macro(list_view.currentIndex)
	}
	
	function on_delete() {
		get_stack_page().on_delete_macro(list_view.currentIndex)
	}
	
	function on_rename() {
		get_stack_page().on_rename_macro(list_view.currentIndex)
	}
	
	function on_stop() {
		get_stack_page().on_stop()
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
            onClicked: {
                on_add()
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
            text: "rename"
            onClicked: {
                on_rename()
            }
        }
        
        Rectangle {
            color: "transparent"
            height: app_settings.action_bar_height
            width: app_settings.button_spacing
        }
        AppLabelButton {
            text: "stop"
            onClicked: {
                on_stop()
            }
        }
        Rectangle {
            color: "transparent"
            height: app_settings.action_bar_height
            width: app_settings.button_spacing
        }
        AppLabelButton {
            text: "delete"
            onClicked: {
                on_delete()
            }
        }
        Item {Layout.fillWidth: true}
    }
}

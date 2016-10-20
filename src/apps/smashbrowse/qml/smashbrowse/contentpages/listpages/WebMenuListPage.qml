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
import smashbrowse.appwidgets 1.0
import smashbrowse.contentpages.listmodels 1.0

BaseListPage {
    id: page
    
        // Buttons.
    RowLayout {
    	Layout.maximumWidth: parent.width
    	anchors.bottom: parent.bottom

        Item {Layout.fillWidth: true}
        AppLabelButton {
            text: "add"
            onClicked: {
                var path = edit_string_page.Stack.view.get_title_path(1, edit_string_page.Stack.view.depth)
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
            	var path = edit_string_page.Stack.view.get_title_path(1, edit_string_page.Stack.view.depth)
                on_edit(path)
            }
        }
        Rectangle {
            color: "transparent"
            height: app_settings.action_bar_height
            width: app_settings.button_spacing
        }
        AppLabelButton {
            text: "remove"
            onClicked: {
                var path = edit_string_page.Stack.view.get_title_path(1, edit_string_page.Stack.view.depth)
                on_remove(path)
            }
        }
        Item {Layout.fillWidth: true}
    }
}

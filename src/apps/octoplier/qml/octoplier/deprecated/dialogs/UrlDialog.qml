import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4



import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

import octoplier.appwidgets 1.0

Dialog {
    id: url_dialog
    title: "Enter URL"
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    height: 100

    // Access.
    property alias text_field: urlField

    // Dependencies.
    property var web_browser_page
    property var copy_paste_bar
        
    // URL Text Entry Field.
    AppTextField {
        id: urlField
        tool_bar: url_dialog.copy_paste_bar
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: app_settings.action_bar_left_margin
            rightMargin: app_settings.action_bar_right_margin
        }
        
        //text: web_browser_page.web_view_alias.url
        onAccepted: web_browser_page.web_view_alias.url = utils.url_from_input(text)
    }
}


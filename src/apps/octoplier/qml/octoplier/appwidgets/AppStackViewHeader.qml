import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import octoplier.appconfig 1.0
import octoplier.stackedpages 1.0
import octoplier.appwidgets 1.0
import octoplier.menumodels 1.0

// The Stack View Header
Rectangle {
    id: stack_view_header
    height: app_settings.action_bar_height
    width: app_settings.page_width

    x: 0
    y: 0
    z: 0

    color: app_settings.menu_stack_header_bg_color
    border.width: app_settings.menu_stack_header_border_width
    border.color: app_settings.menu_stack_header_border_color
    radius: app_settings.menu_stack_header_radius

    // Dependencies.
    property var stack_view
    property var allow_back_to_last_mode: true // Allows the stack page to hide and go back to previous mode when the back_button is pressed.
    
    // Settings.
    property alias title_text: title_text.text

    // Internal state.
    property var header_title_stack: []

    function push_header_title(title) {
        header_title_stack.push(title)
        title_text.text = header_title_stack[header_title_stack.length-1];
    }

    function pop_header_title() {
        header_title_stack.pop()
        title_text.text = header_title_stack[header_title_stack.length-1];
    }

    function clear_header_titles() {
        header_title_stack = []
    }

    AppImageButton {
        id: back_button
        anchors.verticalCenter: parent.verticalCenter
        x: app_settings.action_bar_left_margin

        tooltip_text: "Back up"
        image_url: "qrc:///icons/ic_arrow_back_white_48dp.png"
        visible: allow_back_to_last_mode ? 1 : (stack_view.depth > 1 ? 1 : 0)
        onClicked: {
            if (stack_view.depth > 1) {
                stack_view.pop_page()
            } else if (!allow_back_to_last_mode) {
                return
            } else {
                // Backing out of stack page, to previous mode page.
                main_bar.switch_to_last_mode()
            }
        }
    }

    Text {
        id: title_text
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        text: "Value Operators"
        font.pointSize: app_settings.font_point_size
        font.italic: false
        color: "white"
    }
}

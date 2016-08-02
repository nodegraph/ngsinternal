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
    property var stack_page

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
        //opacity: stack_view.depth > 1 ? 1 : 0
        onClicked: {
            if (stack_page.stack_view.depth > 1) {
                stack_page.stack_view.pop_model()
            } else {
                stack_page.visible = false
                if (stack_page.last_mode) {
                    main_bar.on_switch_to_mode(stack_page.last_mode)
                }
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

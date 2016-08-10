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

// The Stack View Header displays a title and a back button.
Rectangle {
    id: stack_view_header

    // Geometry.
    height: app_settings.action_bar_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Appearnace.
    color: app_settings.menu_stack_header_bg_color
    border.width: app_settings.menu_stack_header_border_width
    border.color: app_settings.menu_stack_header_border_color
    radius: app_settings.menu_stack_header_radius

    // Dependencies.
    property var stack_view

    // Settings.
    property alias title_text: title_text.text
    property var show_back_button: true

    AppImageButton {
        id: back_button
        anchors {
        	verticalCenter: parent.verticalCenter
    	}
        
        // Positioning.
        x: app_settings.page_left_margin

        tooltip_text: "Back up"
        image_url: "qrc:///icons/ic_arrow_back_white_48dp.png"
        visible: show_back_button
        onClicked: {
            if (stack_view.depth > 1) {
                stack_view.pop_page()
            } else {
                main_bar.switch_to_current_mode()
            }
        }
    }

    Text {
        id: title_text
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        text: "Hello"
        font.pointSize: app_settings.font_point_size
        font.italic: false
        color: "white"
    }
}

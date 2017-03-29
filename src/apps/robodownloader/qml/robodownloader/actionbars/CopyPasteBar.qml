import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import robodownloader.appwidgets 1.0

Rectangle {
    id: copy_paste_bar
    
    // Dimensions.
    height: app_settings.action_bar_height
    width: app_settings.action_bar_width
    
    // Positioning.
    x: app_settings.action_bar_x
    y: app_settings.action_bar_y
    z: app_settings.action_bar_z
    
    // Public Properties.
    property var text_editor;
    
    // Visibility.
    opacity: 1
    visible: true
    
    // Color.
    color: app_settings.action_bar_bg_color

    function on_virtual_keyboard_visibility_changed() {
        if (Qt.inputMethod.visible) {
            console.log("qt inputmethod visible is true")
            visible = true
        } else {
            console.log("qt inputmethod visible is false")
            visible = false
        }
    }
    
    // Back Button.
    AppImageButton {
        id: back_button
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: app_settings.action_bar_left_margin
    		rightMargin: app_settings.action_bar_right_margin
        }

        tooltip_text: "Back"
        image_url: "qrc:///icons/ic_arrow_back_white_48dp.png"

        onClicked: {
            text_editor.deselect()
            copy_paste_bar.visible = false
        }
    }

    // Select All Button.
    AppImageButton {
        id: select_all_button
        anchors {
            verticalCenter: parent.verticalCenter
            right: cut_button.left
            leftMargin: app_settings.action_bar_left_margin
    		rightMargin: app_settings.action_bar_right_margin
        }
        
        tooltip_text: "Select all"
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        
        onClicked: {
            text_editor.selectAll()
        }
    }
    
    // Cut Button.
    AppImageButton {
        id: cut_button
        anchors {
            verticalCenter: parent.verticalCenter
            right: copy_button.left
            leftMargin: app_settings.action_bar_left_margin
    		rightMargin: app_settings.action_bar_right_margin
        }
        
        tooltip_text: "Cut"
        image_url: "qrc:///icons/ic_content_cut_white_48dp.png"

        onClicked: {
            text_editor.cut()
        }
    }
    
    // Copy Button.
    AppImageButton {
        id: copy_button
        anchors {
            verticalCenter: parent.verticalCenter
            right: paste_button.left
            leftMargin: app_settings.action_bar_left_margin
    		rightMargin: app_settings.action_bar_right_margin
        }
    
        tooltip_text: "Copy"
        image_url: "qrc:///icons/ic_content_copy_white_48dp.png"
    
        onClicked: {
            text_editor.copy()
        }
    }
    
    // Paste Button.
    AppImageButton {
        id: paste_button
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            leftMargin: app_settings.action_bar_left_margin
    		rightMargin: app_settings.action_bar_right_margin
        }
    
        tooltip_text: "Paste"
        image_url: "qrc:///icons/ic_content_paste_white_48dp.png"
    
        onClicked: {
            text_editor.paste()
        }
    }
        
}

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TextField {
    id: text_field
    
    // Dependencies.
    property var tool_bar

    // Dimensions.
    height: app_settings.text_field_height

    anchors {
        left: parent.left
        right: parent.right
        leftMargin: app_settings.action_bar_left_margin
        rightMargin: app_settings.action_bar_right_margin
    }
    
    // Input hints.
    inputMethodHints: Qt.ImhPreferLowercase   // Qt.ImhMultiLine | Qt.ImhEmailCharactersOnly | Qt.ImhUrlCharactersOnly
    
    // Text.
    text: "hello"
    textColor: "white"
    font.pointSize: app_settings.font_point_size
    
    onSelectionStartChanged: {
        if (activeFocus) {
            tool_bar.text_editor = text_field
            tool_bar.visible = true
        }
    }
    
    onSelectionEndChanged: {
        if (activeFocus) {
            tool_bar.text_editor = text_field
            tool_bar.visible = true
        }
    }
        
    onSelectedTextChanged: {
        if (activeFocus) {
            tool_bar.text_editor = text_field
            tool_bar.visible = true
        }
    }   
        
    onActiveFocusChanged: {
        if (activeFocus) {
            tool_bar.text_editor = text_field
            tool_bar.visible = true
        }
    }
        
    onAccepted: {
        tool_bar.visible = false
        tool_bar.text_editor = null
    }
        
    onVisibleChanged: {
        if (!parent.visible) {
            if (tool_bar.text_editor == text_field) {
                tool_bar.visible = false
                tool_bar.text_editor = null
            }
        }
    }
        
    // When the TextFieldStyle is present, the selection handles don't
    // show up on Android. So we comment these out for now.
        
    // Our Style.
    style: TextFieldStyle {

        // Text Properties.
        textColor: "white"
        font.pointSize: app_settings.font_point_size

        // Background with border.
        background: Rectangle {
            radius: app_settings.text_field_radius
            height: app_settings.text_field_height
            border.color: "#FFFFFFFF"
            border.width: app_settings.text_field_border_size
            color: "#FF01579B" //app_settings.action_bar_bg_color
        }
    }
        
    // You should implement these slots when instancing this component.
    // onAccepted: {} -- called when return is pressed or when accept/go button is pressed on android.
}

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TextField {
    id: text_field
    
    // Dependencies.
    property var tool_bar
    property bool use_tool_bar: false

    // Dimensions.
    height: app_settings.text_field_height
    
//    // Input hints.
//    inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhPreferLowercase | Qt.ImhSensitiveData | Qt.ImhLowercaseOnly | Qt.ImhUrlCharactersOnly // Qt.ImhMultiLine | Qt.ImhEmailCharactersOnly | Qt.ImhUrlCharactersOnly
    focus: true
    activeFocusOnPress: true
    inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhPreferLowercase
    echoMode: TextInput.Normal


    // Text.
    text: "hello"
    textColor: "white"
    font.pointSize: app_settings.font_point_size
    
    onSelectionStartChanged: {
        if (use_tool_bar && activeFocus) {
            forceActiveFocus()
            tool_bar.text_editor = text_field
            tool_bar.visible = true
        }
    }
    
    onSelectionEndChanged: {
        if (use_tool_bar && activeFocus) {
            forceActiveFocus()
            tool_bar.text_editor = text_field
            tool_bar.visible = true
        }
    }
        
    onSelectedTextChanged: {
        if (use_tool_bar && activeFocus) {
            forceActiveFocus()
            tool_bar.text_editor = text_field
            tool_bar.visible = true
        }
    }
        
    onActiveFocusChanged: {
        if (use_tool_bar && activeFocus) {
            forceActiveFocus()
            tool_bar.text_editor = text_field
            tool_bar.visible = true
        }
    }
        
    onAccepted: {
        if (use_tool_bar) {
            tool_bar.visible = false
            tool_bar.text_editor = null
        }
    }
        
    onVisibleChanged: {
        if (use_tool_bar && !parent.visible) {
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
            color: app_settings.prompt_color
        }
    }
        
    // You should implement these slots when instancing this component.
    // onAccepted: {} -- called when return is pressed or when accept/go button is pressed on android.
}

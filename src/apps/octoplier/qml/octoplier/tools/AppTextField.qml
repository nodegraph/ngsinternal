import QtQuick 2.6
import QtQuick.Controls 1.4

TextField {
    id: urlField
    
    // Dependencies.
    property var copy_paste_bar

    // Dimensions.
    height: app_settings.text_field_height
    
    // Input hints.
    inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhPreferLowercase
    
    // Text.
    text: "hello"
    textColor: "white"
    font.pointSize: app_settings.font_point_size
    
    onSelectionStartChanged: {
        if (activeFocus) {
            copy_paste_bar.text_editor = urlField
            copy_paste_bar.visible = true
        }
    }
    
    onSelectionEndChanged: {
        if (activeFocus) {
            copy_paste_bar.text_editor = urlField
            copy_paste_bar.visible = true
        }
    }
        
    onSelectedTextChanged: {
        if (activeFocus) {
            copy_paste_bar.text_editor = urlField
            copy_paste_bar.visible = true
        }
    }   
        
    onActiveFocusChanged: {
        if (activeFocus) {
            console.log("urlField is: " + urlField)
            console.log("copy_paste_bar is: " + copy_paste_bar)
            copy_paste_bar.text_editor = urlField
            copy_paste_bar.visible = true
        }
    }
        
    onAccepted: {
        copy_paste_bar.visible = false
        //copy_paste_bar.text_editor = null
        // Move the focus else where
        empty_page.forceActiveFocus();
    }
        
//    onVisibleChanged: {
//        if (!parent.visible) {
//            if (copy_paste_bar.text_editor == urlField) {
//                copy_paste_bar.visible = false
//                copy_paste_bar.text_editor = null
//            }
//        }
//    }
        
    // When the TextFieldStyle is present, the selection handles don't
    // show up on Android. So we comment these out for now.
        
    // Our Style.
//    style: TextFieldStyle {
//        
//        // Text Properties.
//        textColor: "white"
//        font.pointSize: app_settings.font_point_size
//
//        // Background with border.
//        background: Rectangle {
//            radius: app_settings.text_field_radius
//            height: app_settings.text_field_height
//            border.color: "#FFFFFFFF"
//            border.width: app_settings.text_field_border_size
//            color: app_settings.action_bar_bg_color
//        }
//    }
        
    // You should implement these slots when instancing this component.
    // onAccepted: {} -- called when return is pressed or when accept/go button is pressed on android.
}

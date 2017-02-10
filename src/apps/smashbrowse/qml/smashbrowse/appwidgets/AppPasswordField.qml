import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TextField {
    id: password_field
    
    // Dimensions.
    height: app_settings.text_field_height
    width: parent.width * 0.666

    // Input.
    inputMethodHints: Qt.ImhPreferLowercase | Qt.ImhSensitiveData | Qt.ImhHiddenText
    text: ""
    textColor: "white"
    font.pointSize: app_settings.font_point_size
    echoMode: true ? TextInput.Password : TextInput.Normal

    // Center the text.
    horizontalAlignment: TextInput.AlignHCenter
    verticalAlignment: TextInput.AlignVCenter
    
    // Callback to call when the Submit/Enter key is pressed.
    // The callback should accept one argument which is the password.
    property var return_pressed_callback: null
    property var return_released_callback: null
    Keys.onPressed: {
    	if ((event.key == Qt.Key_Return) || (event.key == Qt.Key_Enter)) {
	    	if (return_pressed_callback) {
	        	return_pressed_callback(password_field.text)
	        }
        }
    }
    Keys.onReleased: {
        if ((event.key == Qt.Key_Return) || (event.key == Qt.Key_Enter)) {
	    	if (return_released_callback) {
	        	return_released_callback(password_field.text)
	        }
        }
    }

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
}

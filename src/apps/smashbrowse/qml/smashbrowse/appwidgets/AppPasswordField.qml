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

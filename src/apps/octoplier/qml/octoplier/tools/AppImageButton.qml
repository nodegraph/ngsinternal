import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4


// Relies on an AppSettings object with id: app_settings.

Button {
    id: image_button
    
    // Public Properties.
    property url image_url
    property string tooltip_text: "no tooltip!"
    
    // Dimensions.
    width: app_settings.image_button_width
    height: app_settings.image_button_height
    
    // Anchors.
    // These are used when the AppImageButton is used inside a non-layout object, like a Rectangle.
    anchors.leftMargin: app_settings.action_bar_left_margin
    anchors.rightMargin: app_settings.action_bar_right_margin
    
    // Image.
    Image {
        source: image_url
        width: parent.height * 1
        height: parent.height * 1
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

    // Style.
//    style: ButtonStyle {
//        background: Rectangle {
//            border.width: control.activeFocus ? 2 : 0
//            border.color: "#888"
//            radius: 0
//            color: control.pressed ? app_settings.action_bar_pressed_color : app_settings.action_bar_bg_color
//        }
//    }

    style: ButtonStyle {
        background: Rectangle { color: "transparent" }
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPressAndHold: {
            mouse.accepted = true
            app_tooltip.show(image_button,tooltip_text)
            app_settings.vibrate()
        }
        onClicked: {
            mouse.accepted = false
            image_button.clicked(mouse)
        }
        onReleased: {
            app_tooltip.hide();
            mouse.accepted = false
        }

    }

//    // Layout attached properties.
//    // These are used when the AppImageButton is used inside a Row/ColumnLayout.
//    Layout.fillHeight: true
//    Layout.fillWidth: true
//    Layout.preferredWidth: app_settings.image_button_width
//    Layout.preferredHeight: app_settings.image_button_height
//    Layout.minimumWidth: app_settings.image_button_width
//    Layout.minimumHeight: app_settings.image_button_height
//    Layout.maximumWidth: app_settings.image_button_width
//    Layout.maximumHeight: app_settings.image_button_height
//    Layout.leftMargin: app_settings.action_bar_left_margin
//    Layout.rightMargin: app_settings.action_bar_right_margin
}

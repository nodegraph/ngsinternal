import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

Button {
    id: image_button

    // Public Properties.
    property url image_url
    property string tooltip_text: "no tooltip!"
    property bool lit: false

    // Dimensions.
    width: app_settings.image_button_width
    height: app_settings.image_button_height

    // Image.
    Image {
        id: button_image
        source: image_url
        width: parent.height * 1
        height: parent.height * 1
        z:1
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

    style: ButtonStyle {
        background: Rectangle {
            color: (mouse_area.pressed || lit) ? app_settings.image_button_press_color : app_settings.action_bar_bg_color
            radius: app_settings.image_button_radius
        }
    }

    MouseArea {
        id: mouse_area
        anchors.fill: parent
        onPressAndHold: {
            mouse.accepted = true
            if (app_tooltip.visible == false) {
	            app_tooltip.show(image_button,tooltip_text)
	            app_settings.vibrate()
            }
        }
        onPressed: {
            app_settings.vibrate()
            mouse.accepted = true
            image_button.clicked(mouse)
        }
        onReleased: {
            app_tooltip.hide();
            mouse.accepted = true
        }
        onExited: {
            app_tooltip.hide();
        }

    }
}

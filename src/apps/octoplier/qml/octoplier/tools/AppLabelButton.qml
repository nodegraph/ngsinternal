import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.tools 1.0

Button {
    id: label_button

    // Public Properties.
    property string tooltip_text: "no tooltip!"

    // Dimensions.
    //width: app_settings.image_button_width
    height: app_settings.action_bar_height

    // Anchors.
    // These are used when the AppImageButton is used inside a non-layout object, like a Rectangle.
    anchors.leftMargin: app_settings.action_bar_left_margin
    anchors.rightMargin: app_settings.action_bar_right_margin

    style: ButtonStyle {
        background: Rectangle {
            color: mouse_area.pressed ? app_settings.image_button_press_color : app_settings.action_bar_bg_color
            radius: app_settings.image_button_radius
            border.width: app_settings.text_field_border_size
            border.color: "white"
        }
        label: Text {
          verticalAlignment: Text.AlignVCenter
          horizontalAlignment: Text.AlignHCenter
          font.family: "Arial" //"Helvetica"
          font.pointSize: app_settings.font_point_size
          color: "white"
          text: " " + control.text + " "
        }
    }

    MouseArea {
        id: mouse_area
        anchors.fill: parent
        onPressAndHold: {
            mouse.accepted = true
            app_tooltip.show(image_button,tooltip_text)
            app_settings.vibrate()
        }
        onPressed: {
            mouse.accepted = true
            label_button.clicked(mouse)
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

import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import robodownloader.appwidgets 1.0

Button {
    id: label_button

    // Public Properties.
    property string tooltip_text: ""
    property alias mouse_area: mouse_area

    // Dimensions.
    //width: app_settings.image_button_width
    height: app_settings.action_bar_height

    // Signals
    signal mouse_pressed()

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
            if (tooltip_text != "") {
                app_tooltip.show(label_button,tooltip_text)
                quick_view.vibrate(10)
            }
        }
        onPressed: {
            mouse.accepted = true
            mouse_pressed()
        }
        onReleased: {
            app_tooltip.hide();
            mouse.accepted = true
            label_button.clicked(mouse)
        }
        onExited: {
            app_tooltip.hide();
        }
    }

}

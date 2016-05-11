import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.tools 1.0

Label {
    id: app_label
    text: "Hello world"
    font.pixelSize: app_settings.font_point_size
    font.italic: false
    color: "white"

    anchors {
        left: parent.left
        right: parent.right
        leftMargin: app_settings.action_bar_left_margin
        rightMargin: app_settings.action_bar_right_margin
    }
}

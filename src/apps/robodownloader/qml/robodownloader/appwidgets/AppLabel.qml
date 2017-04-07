import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import robodownloader.appwidgets 1.0

Label {
    id: app_label
    text: "Hello world"
    font.pointSize: app_settings.font_point_size
    font.italic: false
    color: "white"
}

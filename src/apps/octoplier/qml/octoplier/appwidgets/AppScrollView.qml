import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


ScrollView {
    id: scroll_view

    // Dimensions.
    height: app_settings.page_height - app_settings.action_bar_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Scrolling Policy.
    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
    verticalScrollBarPolicy: (Qt.platform.os == "android") || (Qt.platform.os == "ios") ? Qt.ScrollBarAlwaysOff : Qt.ScrollBarAsNeeded
}


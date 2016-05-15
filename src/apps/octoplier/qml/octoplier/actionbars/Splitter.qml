import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

// --------------------------------------------
// Splitter and Progress Bar.
// --------------------------------------------

Rectangle {
    id: splitter
    color: "#FFFFFFFF"
    property alias progress_bar: our_progress_bar

    // Dimensions.
    height: app_settings.splitter_height
    width: app_settings.page_width

    // Positioning.
    x: app_settings.page_x
    y: app_settings.action_bar_height
    z: app_settings.page_z

    // URL loading progress overlay.
    ProgressBar {
        id: our_progress_bar
        anchors.fill: parent
        z: 1
        visible: web_browser_page.web_view_alias.loading
        minimumValue: 0
        maximumValue: 100
        value: web_browser_page.web_view_alias.loadProgress >= 100 ? 0 : web_browser_page.web_view_alias.loadProgress

        style: ProgressBarStyle {
            background: Rectangle {
                color: "transparent"
            }
            progress: Rectangle {
                anchors.fill: parent
                color: "#FFFF0000"
            }
        }
    }
}


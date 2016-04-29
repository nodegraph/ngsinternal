import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
Window {
    id: splash_page
    property int num_components: 0
    property int progress: 0

    // Behavior.
    modality: Qt.ApplicationModal
    flags: Qt.Dialog | Qt.WindowStaysOnTopHint  //Qt.SplashScreen

    // Background Appearance.
    color: "#FF03A9F4"
    visible: true

    // Sizing.
    x: 0
    y: 0
    width: Screen.width
    height: Screen.height

    // Methods
    function hide() {
        visible = false
        flags = Qt.Dialog | Qt.WindowStaysOnBottomHint
    }
    function done() {
        if (progress == 2 * num_components) {
            return true
        }
        return false
    }

    function on_start_loading() {
        progress++
        status.text = progress + "/" + 2 * num_components
        cpp_bridge.process_events()
        //update()
        console.log("progress: "+progress)
    }
    function on_finished_loading() {
        progress++
        status.text = progress + "/" + 2 * num_components
        //cpp_bridge.process_events()
        //update()
        console.log("progress: "+progress)
    }

    // Logo.
    Image {
        id: splash_image
        x: 0
        y: 0
        source: "qrc:///images/octopus_blue.png"
    }

//    // Bar.
//    Rectangle {
//        id: progress_bar
//        color: white
//        x:0
//        y:400
//        anchors.bottom: parent.bottom - 3* action_bar_height
//        height: action_bar_height
//        width: (parent.width - app_settings.action_bar_left_margin - app_settings.action_bar_right_margin) * (progress / (2*num_components))
//    }

    // URL loading progress overlay.
    ProgressBar {
        id: our_progress_bar

        // Dimensions.
        height: app_settings.action_bar_height /3.0
        width: parent.width - app_settings.action_bar_left_margin - app_settings.action_bar_right_margin

        // Positioning.
        x: app_settings.action_bar_left_margin
        y: parent.height - 5* app_settings.action_bar_height
        z: app_settings.page_z

        //y: 300
        //x: app_settings.action_bar_left_margin
        //height: appsettings.action_bar_height
        //width: (parent.width - app_settings.action_bar_left_margin - app_settings.action_bar_right_margin) * (progress / (2*num_components))

        //z: 1
        visible: true
        minimumValue: 0
        maximumValue: 2* num_components
        value: progress

        style: ProgressBarStyle {
            background: Rectangle {
                color: "transparent"
            }
            progress: Rectangle {
                anchors.fill: parent
                color: "#FFFFFFFF"
            }
        }
    }

    // Loading Status.
    Label {
        id: status
        // On desktop machines, using the pre-built libraries from QT, the text gets garbled.
        // However is we switch from the default NativeRendering to QtRendering, it displays fine.
        // However we are primarily targetting mobile for now so we leave it at NativeRendering.
        //renderType: Text.QtRendering //Text.NativeRendering
        font.pointSize: app_settings.font_point_size
        font.italic: false
        color: "white"
        //anchors.top: splash_image.bottom
        //anchors.bottom: parent.bottom
        y: parent.height - 3* app_settings.action_bar_height
        anchors.left: parent.left
        anchors.right: parent.right
        horizontalAlignment: Qt.AlignHCenter
    }
}

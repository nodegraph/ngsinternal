import QtQuick 2.7
import QtQuick.Controls 1.5

Rectangle {
    id: splash_page
    anchors.fill: parent
    visible: true

    // This is hard coded because the AppSettings has not been loaded yet.
    color: "#FF0288D1" // a darker blue than the node graph blue

    property string ngs_version: ""
    property string app_name: ""

    // Logo.
    Image {
        id: splash_image
        width: parent.width / 3
        height: width
        x: width
        y: parent.height * 3/10
        source: "qrc:///images/robot_white.png"
    }

    Label {
        font.pointSize: 50
        font.italic: false
        color: "white"

        y: parent.height * 6.5/10
        text: app_name
        anchors {
            horizontalCenter:  parent.horizontalCenter
        }
    }

    // Version Label.
    Label {
        font.pointSize: 25
        font.italic: false
        color: "white"

        y: parent.height * 7.5/10
        text: "Version " + ngs_version
        anchors {
            horizontalCenter:  parent.horizontalCenter
        }
    }

}

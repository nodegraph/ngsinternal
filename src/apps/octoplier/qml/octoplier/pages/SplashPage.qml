import QtQuick 2.6

Rectangle {
    id: splash_page
    anchors.fill: parent
    visible: true

    // This is hard coded because the AppSettings has not been loaded yet.
    color: "#FF0288D1" // a darker blue than the node graph blue

    // Logo.
    Image {
        id: splash_image
        width: parent.width / 3
        height: width
        x: width
        y: (parent.height - width) / 2
        source: "qrc:///images/octopus_white.png"
    }

}

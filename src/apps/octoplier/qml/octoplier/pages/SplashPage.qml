import QtQuick 2.6

Rectangle {
    id: splash_page
    anchors.fill: parent
    visible: true

    color: "#FF03A9F4"

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

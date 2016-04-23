import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import QtWebView 1.1

//import QtQuick 2.0
import SceneGraphRendering 1.0

ApplicationWindow {
    title: qsTr("todopile")
    width: 640
    height: 480
    visible: true

    Component.onCompleted: {
        deviceItem.build_test_graph()
    }

//    menuBar: MenuBar {
//        Menu {
//            title: qsTr("&File")
//            MenuItem {
//                text: qsTr("&Open")
//                onTriggered: message_dialog.show(qsTr("Open action triggered"));
//                //onTriggered: message_dialog.show(myObject.temp);
//            }
//            MenuItem {
//                text: qsTr("E&xit")
//                onTriggered: Qt.quit();
//            }
//        }
//    }

//    MainForm {
//        anchors.fill: parent
//        button1.onClicked: message_dialog.show(qsTr("Button 1 pressed"))
//        button2.onClicked: message_dialog.show(qsTr("Button 2 pressed"))
//        button3.onClicked: message_dialog.show(qsTr("Button 3 pressed"))
//    }

    ListModel {
        id: testModel
        ListElement {
            name: "Bill Smith"
            number: "555 3264"
        }
        ListElement {
            name: "John Brown"
            number: "555 8426"
        }
        ListElement {
            name: "Sam Wise"
            number: "555 0473"
        }
    }

    Rectangle {
        id: action_bar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 200
        color: "blue"

        WebView {
            id: web_view
            objectName: "web_view_object"

            anchors.fill: parent
            url: "http://www.google.com"

        }
    }

    NodeGraphQuickItem {
        id: deviceItem
        anchors.top: action_bar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        opacity: 1
    }




    MessageDialog {
        id: message_dialog
        title: qsTr("May I have your attention, please?")

        function show(caption) {
            message_dialog.text = caption;
            message_dialog.open();
        }
    }
}


//Item {
//    width: 1024
//    height: 1024
//}

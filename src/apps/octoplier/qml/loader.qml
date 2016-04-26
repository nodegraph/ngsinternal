
import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import NodeGraphRendering 1.0
import octoplier.actionbars 1.0
import octoplier.pages 1.0
//import octoplier.dialogs 1.0
//import octoplier.menus 1.0
import octoplier.tools 1.0
//import octoplier.apis 1.0
import octoplier.config 1.0

ApplicationWindow {
    id: app
    title: qsTr("octoplier")
    width: 640
    height: 480
    visible: true

    Component.onCompleted: {
        node_graph_page.node_graph.build_test_graph()
        //device_item.build_test_graph()
        console.log("ApplicationWindow dim: ", app.width, "," , app.height)
    }

    // Global App Objects.
    AppUnits {
        id: app_units
    }

    AppSettings {
        id: app_settings
    }

    MainBar {
        id: main_bar
    }

//    CopyPasteBar {
//        id: copy_paste_bar
//    }

//    Rectangle {
//        id: main_bar
////        // Dimensions.
////        height: 200
////        width: parent.width

////        // Positioning.
////        x: 0
////        y: 0
////        z: 0

//        // Dimensions.
//        height: app_settings.action_bar_height
//        width: app_settings.action_bar_width+10

//        // Positioning.
//        x: app_settings.action_bar_x
//        y: app_settings.action_bar_y
//        z: app_settings.action_bar_z

//        color: "yellow"

//    }

    NodeGraphPage {
        id: node_graph_page
    }


//    Rectangle {
//        id: node_graph_page
//        //property alias node_graph: our_node_graph

//        // Dimensions.
//        height: app_settings.page_height
//        width: app_settings.page_width

//        // Positioning.
//        x: app_settings.page_x
//        y: app_settings.page_y
//        z: app_settings.page_z

//        NodeGraphQuickItem {
//            id: our_node_graph
//            anchors.fill: parent

//    //        anchors.top: main_bar.bottom
//    //        anchors.left: parent.left
//    //        anchors.right: parent.right
//    //        anchors.bottom: parent.bottom

//    //        z: 0

//            visible: true
//            opacity: 0.1
//        }

//    }

//    NodeGraphQuickItem {
//        id: device_item
//        //anchors.fill: parent
//        //color: "yellow"

//        // Dimensions.
//        height: app_settings.page_height
//        width: app_settings.page_width

//        // Positioning.
//        x: app_settings.page_x
//        y: app_settings.page_y
//        z: app_settings.page_z

////        anchors.top: main_bar.bottom
////        anchors.left: parent.left
////        anchors.right: parent.right
////        anchors.bottom: parent.bottom

////        z: 0

//        visible: true
//        opacity: 0.9
//    }





}

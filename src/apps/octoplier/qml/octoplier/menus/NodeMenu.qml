import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import octoplier.config 1.0
import octoplier.menus 1.0
import octoplier.pages 1.0
import octoplier.tools 1.0
import octoplier.data 1.0

//Menu {
    
//    // Public Properties.
//    title: "Node Context Menu"

//    // Methods.
//    function on_popup_menu() {
//        app_settings.vibrate()
//        popup()
//    }

//    MenuItem {
//        text: "View"
//        onTriggered: {
//            var test = {};
//            test.aaa = "hello";
//            test.bbb = 1;
//            test.ccc = 1.123;
//            test.ddd = [1,2,3];
//            test.eee = ["hello",1,2,{a:1, b:2, c:{xxx: "hi", yyy: "there"}}];
//            cpp_bridge.on_test_1(test);
//        }
//    }
    
//    MenuItem {
//    	text: "Edit"
//    }
    
//    MenuItem {
        
//    }
//}



Rectangle {
    id: menu_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    color: app_settings.ng_bg_color

    Component {
        id: list_view_delegate
        Rectangle {
            id: delegate
            width: parent.width;
            height: app_settings.list_item_height_large
            border.color: "white"
            border.width: app_settings.list_item_border_width
            color: "transparent"
            Row {
                Rectangle {
                    height: app_settings.list_item_height_large
                    width: app_settings.list_item_height_large
                    color: "transparent"
                    Image {
                        height: parent.height * 2/3
                        width: parent.height * 2/3
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: image_url
                    }
                }

                Column {
                    Text {
                        text: '<b>' + title + '</b>'
                        font.pointSize: app_settings.font_point_size
                        font.italic: false
                        color: "white"
                    }
                    Text {
                        text: description
                        font.pointSize: app_settings.font_point_size
                        font.italic: false
                        color: "white"
                    }
                }
            }
            MouseArea {
                id: mousearea1
                anchors.fill: parent
                onPressed: {
                    mouse.accepted = true
                    console.debug("rect click")
                    delegate.ListView.view.currentIndex = index
                    stack_view.push({item: "qrc:///qml/octoplier/pages/MenuPage.qml"})
                }
            }
        }
    }

    Component {
        NodeContextModel {
            ListElement {
                image_url: "qrc:///icons/ic_settings_white_48dp.png"
                title: "Add"
            }
            ListElement {
                image_url: "qrc:///icons/ic_settings_white_48dp.png"
                title: "Add"
            }
            ListElement {
                image_url: "qrc:///icons/ic_settings_white_48dp.png"
                title: "Add"
            }
        }
    }

    ListView {
        // Dimensions.
        height: app_settings.menu_page_height
        width: app_settings.menu_page_width

        // Positioning.
        x: 0
        y: 0
        z: 0

        // CurrentIndex.
        currentIndex: -1

        model: NodeContextModel {}
        //header: list_view_header
        //headerPositioning : ListView.PullBackHeader
        delegate: list_view_delegate
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        highlightMoveDuration: 0
        focus: true
    }

}

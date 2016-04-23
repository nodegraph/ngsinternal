import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQml.Models 2.2

TreeView {
    
    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width
    
    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z
    
    // Signals.
    signal popup_menu()

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.view_node_mode) {
            visible = true;
        } else {
            visible = false;
        }
    }
    function on_open_more_options() {
        if (visible) {
            popup_menu();
        }
    }

    TableViewColumn {
        title: "Name"
        role: "display"
        width: app_settings.page_width
    }
//    TableViewColumn {
//        title: "Value"
//        role: "display"
//        //width: parent.width * 0.0
//    }
    
    alternatingRowColors: true
    backgroundVisible: false
    headerVisible: false
    
    horizontalScrollBarPolicy: (Qt.platform.os == "android") ? Qt.ScrollBarAlwaysOff : Qt.ScrollBarAsNeeded
    verticalScrollBarPolicy: (Qt.platform.os == "android") ? Qt.ScrollBarAlwaysOff : Qt.ScrollBarAsNeeded
    
//    contentHeader: Rectangle {
//        height: 10
//    }
//    contentFooter: Rectangle {
//        height: 10
//    }

    selectionMode: SelectionMode.SingleSelection
    
    model: treemodel

    // This gets around an treeview bug on android:
    // Begin fix: QTBUG-47243 qml treeview can't select item on android.
    selection: ItemSelectionModel {
        id: selModel
            model: treemodel
       }
    onClicked: {
        selModel.clearCurrentIndex();
        selModel.setCurrentIndex(index, 0x0002 | 0x0010);
    }
    // End Fix:
    
    onActivated: {
        console.log("activated " + index.row + "," + index.column)
    }
    
    onExpanded: {
        console.log("expanded " + index.row + "," + index.column + ":: " + index.data)
        cpp_bridge.on_move_root(index.row);
    }
    
    
//    onClicked: {
//        console.log("clicked " + index.row + "," + index.column)
//    }
    
    onPressAndHold: {
        selModel.clearCurrentIndex();
        selModel.setCurrentIndex(index, 0x0002 | 0x0010);
        console.log("pressed " + index.row + "," + index.column)
    }

    style: TreeViewStyle {
        
        indentation: app_settings.tree_view_indentation
        
        headerDelegate: Rectangle {
            height: 0
//            height: app_settings.tree_view_header_height
//            color: app_settings.ng_bg_color
//            Text {
//                id: textItem
//                anchors.fill: parent
//                verticalAlignment: Text.AlignVCenter
//                horizontalAlignment: styleData.textAlignment
//                anchors.leftMargin: app_settings.tree_view_header_left_margin
//                text: styleData.value
//                font.pointSize: app_settings.font_point_size
//                elide: Text.ElideRight
//                color: "#FFFFFFFF"
//                //renderType: Settings.isMobile ? Text.QtRendering : Text.NativeRendering
//            }
//            Rectangle {
//                anchors.right: parent.right
//                anchors.top: parent.top
//                anchors.bottom: parent.bottom
//                anchors.bottomMargin: 1
//                anchors.topMargin: 1
//                width: 1
//                color: "#FFFF0000" //app_settings.action_bar_bg_color
//            }
        }
        
        rowDelegate: Rectangle {
            z: 0
            height: app_settings.tree_view_row_height
            //property color selectedColor: control.activeFocus ? "#07c" : "#999"
            //color: styleData.selected ? selectedColor :
            //                            !styleData.alternate ? alternateBackgroundColor : backgroundColor
            //color: !styleData.alternate ? app_settings.action_bar_pressed_color : app_settings.action_bar_bg_color
            
            color: styleData.selected ? app_settings.tree_view_selected_row_bg_color :
                                        !styleData.alternate ? app_settings.action_bar_pressed_color : app_settings.action_bar_bg_color
        }
        
        branchDelegate: Item {
            width: app_settings.tree_view_twisty_width
            height: app_settings.tree_view_twisty_height
            
            // Drawing the branch icons with special triangle ascii chars
            // doesn't seem to work on android.
//            Text {
//                //visible: styleData.column === 0 && styleData.hasChildren
//                //text: styleData.isExpanded ? "\u25bc" : "\u25b6"
//                text: styleData.isExpanded ? "[-]" : "[+]"
//
//                //color: !control.activeFocus || styleData.selected ? styleData.textColor : "#666"
//
//                color: "white"
//
//                //font.pointSize: 20
//                font.family: (Qt.platform.os=="android") ? "Droid Sans Mono" : "Courier" //"Courier"
//                font.bold: true
//                font.pointSize: app_settings.font_point_size
//
//                //renderType: Text.NativeRendering
//                anchors.centerIn: parent
//                anchors.verticalCenterOffset: styleData.isExpanded ? 2 : 0
//            }
//            Rectangle {
//                color: "#FFFF0000"
//                width: app_settings.tree_view_twisty_width
//                height: app_settings.tree_view_twisty_height
//                anchors.centerIn: parent
//            }
            Image{
                id: closed_image
                source: "qrc://../icons/ic_add_circle_outline_white_48dp.png"
                //transform: Rotation { origin.x: width/2.0; origin.y: height/2.0; angle: 90}
                width: parent.height * 1
                height: parent.height * 1
                anchors.verticalCenter: parent.verticalCenter
                //anchors.horizontalCenter: parent.horizontalCenter
                visible: !styleData.isExpanded
            }
            Image{
                id: opened_image
                source: "qrc://../icons/ic_remove_circle_outline_white_48dp.png"
                //transform: Rotation { origin.x: width/2.0; origin.y: height/2.0; angle: 90}
                width: parent.height * 1
                height: parent.height * 1
                anchors.verticalCenter: parent.verticalCenter
                //anchors.horizontalCenter: parent.horizontalCenter
                visible: styleData.isExpanded
            }
        }
        
        itemDelegate: Item {
            Text {
                id: item_delegate_text
                anchors.verticalCenter: parent.verticalCenter
                //color: styleData.hasChildren ? "#FFFF0000" : styleData.textColor
                color: app_settings.tree_view_item_text_color
                elide: styleData.elideMode
                // text: styleData.column ? styleData.value : styleData.value[0].toString() + styleData.value[1].toString()
                text: styleData.column == 0 && (styleData.value[0] && !styleData.value[0].isNull) ? styleData.value[0].toString() +
                        (styleData.value[1] && !styleData.value[1].isNull ? " : " + styleData.value[1].toString() : "" ) : ""
                font.pointSize: app_settings.font_point_size
            }
        }
    }

}


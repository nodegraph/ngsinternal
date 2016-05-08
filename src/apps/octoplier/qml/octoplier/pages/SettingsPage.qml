import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import octoplier.menus 1.0
import octoplier.tools 1.0


ScrollView {
    id: page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width
    
    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    // Appearance.
    //color: "pink"
    
    // Scrolling.
    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
    verticalScrollBarPolicy: (Qt.platform.os == "android") ? Qt.ScrollBarAlwaysOff : Qt.ScrollBarAsNeeded

    // Dependencies.
    property var copy_paste_bar

    // Methods.
    function on_switch_to_mode(mode) {
    	if (mode == app_settings.settings_mode) {
            visible = true;
            menu_stack_page.visible = false
            console.log("switching to settings page!")
        } else {
        	visible = false;
        }
    }
            
    Rectangle {
        id: content
        color: app_settings.ng_bg_color

//        width: Math.max(page.viewport.width, column.implicitWidth + 2 * column.spacing)
//        height: Math.max(page.viewport.height, column.implicitHeight + 2 * column.spacing)
        
        width: page.viewport.width
        height: page.viewport.height

        ColumnLayout {
            id: column

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: column.spacing
            
            Label {
                text: "App Password"
                color: "#FFFFFFFF"
                font.pointSize: app_settings.font_point_size
            }
        
            GroupBox {
                Layout.fillWidth: true
                ColumnLayout {
                    anchors.fill: parent
                    TextField {
                        height: app_settings.text_field_height
//                        placeholderText: "Password";
//                        echoMode: TextInput.Password;
                        Layout.fillWidth: true
                        font.pointSize: app_settings.font_point_size
                        textColor: "#FFFFFFFF"
                        }
                }
            }
            
            TextEdit {
                Layout.fillWidth: true
                text: "abc where in the world"
            }
            
            GroupBox {
                //Layout.fillWidth: true
                width: parent.width
                title: "web browser initial url"
                //Layout.fillWidth: true
                    //anchors.fill: parent
                    //TextField {
                    AppTextField {
                        copy_paste_bar: page.copy_paste_bar
                        width: parent.width
//                        anchors.fill: parent
                        //placeholderText: "...";
//                        Layout.fillWidth: true;
//                        z: 1
                        
//                        menu: Component {
//                            id: text_edit_menu
//                            EditMenu {
//                            }
//                        }
                        
//                        menu: EditMenu_ios {}

                        
                        //menu: text_selection_menu_component
//                        MouseArea {
//                            anchors.fill: parent
//                            onPressAndHold: {
//                                text_selection_menu.popup();
//                            }
//                        }
                    }
                    //TextField { placeholderText: "Password"; echoMode: TextInput.Password; Layout.fillWidth: true }
                

                
            }

//            GroupBox {
//                title: "ComboBox"
//                Layout.fillWidth: true
//                ColumnLayout {
//                    anchors.fill: parent
//                    ComboBox {
//                        model: Qt.fontFamilies()
//                        Layout.fillWidth: true
//                    }
//                    ComboBox {
//                        editable: true
//                        model: ListModel {
//                            id: listModel
//                            ListElement { text: "Apple" }
//                            ListElement { text: "Banana" }
//                            ListElement { text: "Coconut" }
//                            ListElement { text: "Orange" }
//                        }
//                        onAccepted: {
//                            if (find(currentText) === -1) {
//                                listModel.append({text: editText})
//                                currentIndex = find(editText)
//                            }
//                        }
//                        Layout.fillWidth: true
//                    }
//                }
//            }
//
//            GroupBox {
//                title: "SpinBox"
//                Layout.fillWidth: true
//                ColumnLayout {
//                    anchors.fill: parent
//                    SpinBox { value: 99; Layout.fillWidth: true; z: 1 }
//                    SpinBox { decimals: 2; Layout.fillWidth: true }
//                }
//            }
        }
    }
}

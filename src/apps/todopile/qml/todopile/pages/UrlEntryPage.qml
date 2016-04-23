import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import todopile.tools 1.0

Rectangle {
    id: url_entry_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width
    
    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z
    
    // Properties.
    color: app_settings.ng_bg_color

    // Dependencies.
    property var web_browser_page
    property var copy_paste_bar

    // Signals.
    signal switch_to_mode(int mode)

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.url_entry_mode) {
            visible = true;
        } else {
            visible = false;
        }
    }

    ColumnLayout {
        y: app_settings.page_y
        
        anchors.left: parent.left
        anchors.right: parent.right
        
        // URL Text Entry Field.
        AppTextField {
            id: urlField
            copy_paste_bar: url_entry_page.copy_paste_bar
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: app_settings.action_bar_left_margin
                rightMargin: app_settings.action_bar_right_margin
            }

            //text: web_browser_page.web_view_alias.url
                
            onAccepted: {
                switch_to_mode(app_settings.web_browser_mode)
                web_browser_page.web_view_alias.url = utils.url_from_input(urlField.text)
            }
        }
        
//        // Buttons.
//        RowLayout {
//            Item {Layout.fillWidth: true}
//            Button {
//                text: "ok"
//                onClicked: {
//                   switch_to_mode(app_settings.web_browser_mode);
//                    web_browser_page.web_view_alias.url = utils.url_from_input(urlField.text)
//                }
//            }
//            Button {
//                text: "cancel"
//                onClicked: { 
//                    switch_to_mode(app_settings.web_browser_mode);
//                }
//            }
//            Item {Layout.fillWidth: true}
//        }
    }
    }
    
    

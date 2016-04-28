import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.tools 1.0

Rectangle {
    id: main_bar

    // Dimensions.
    height: app_settings.action_bar_height
    width: parent.width //app_settings.action_bar_width
    
    // Positioning.
    x: app_settings.action_bar_x
    y: app_settings.action_bar_y
    z: app_settings.action_bar_z
    
    // Signals.
    signal switch_to_mode(int mode)
    signal open_more_options()

    Component.onCompleted: {
        console.log("mainbar dim: ", main_bar.width, "," , main_bar.height)
    }
    
    // Methods.
    // Set the current mode of the action bar.
    function on_switch_to_mode(m) {
        if (m == app_settings.node_graph_mode) {
            action_bar_title.text = "Nodes"
            more_menu_button.visible = true;
        } else if (m == app_settings.web_browser_mode) {
            action_bar_title.text = "Browser"
            more_menu_button.visible = true;
        } else if (m == app_settings.posts_mode) {
            action_bar_title.text = "Posts"
            more_menu_button.visible = false;
        } else if (m == app_settings.settings_mode) {
            action_bar_title.text = "Settings"
            more_menu_button.visible = false;
        } else if (m == app_settings.view_node_mode) {
            action_bar_title.text = "Nodal Output"
            more_menu_button.visible = true;
        } else if (m == app_settings.edit_node_mode) {
            action_bar_title.txt = "Nodal Params"
            more_menu_button.visible = false;
        } else if (m == app_settings.url_entry_mode) {
            action_bar_title.text = "Url Entry"
            more_menu_button.visible = false;
        }
        switch_to_mode(m)
    }
    
    // Background Setup.
    color: app_settings.action_bar_bg_color

    // Mode Title.
    Label {
        id: action_bar_title
        // On desktop machines, using the pre-built libraries from QT, the text gets garbled.
        // However is we switch from the default NativeRendering to QtRendering, it displays fine.
        // However we are primarily targetting mobile for now so we leave it at NativeRendering.
        //renderType: Text.QtRendering //Text.NativeRendering
        font.pointSize: app_settings.font_point_size
        font.italic: false
        color: "white"
        anchors.left: parent.left
        anchors.leftMargin: app_settings.action_bar_left_margin
        anchors.verticalCenter: parent.verticalCenter
    }
    
    AppImageButton {
        id: more_menu_button
        
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: node_graph_button.left
        
        image_url: "qrc:///icons/ic_more_vert_white_48dp.png"
        tooltip_text: "More actions"
        
        onClicked: {
            open_more_options();
        }
    }

    // Node Graph Mode Button.
    AppImageButton {
        id: node_graph_button
        
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: browser_button.left
        
        image_url: "qrc:///icons/ic_widgets_white_48dp.png"
        tooltip_text: "Nodes"
        
        onClicked: {
            on_switch_to_mode(app_settings.node_graph_mode)
        }
    }

    // Web Browser Mode Button.
    AppImageButton {
        id: browser_button
        
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: posts_button.left
        
        image_url: "qrc:///icons/ic_public_white_48dp.png"
        tooltip_text: "Browser"
        
        onClicked: {
            on_switch_to_mode(app_settings.web_browser_mode)
        }
    }

    // Hot Posts Mode Button.
    AppImageButton {
        id: posts_button
        
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: settings_button.left
        
        image_url: "qrc:///icons/ic_whatshot_white_48dp.png"
        tooltip_text: "Posts"
        
        onClicked: {
            on_switch_to_mode(app_settings.posts_mode)
        }
    }

    // Settings Mode Button.
    AppImageButton {
        id: settings_button
        
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
            tooltip_text: "Settings"
        
        onClicked: {
            on_switch_to_mode(app_settings.settings_mode)
        }
    }
}



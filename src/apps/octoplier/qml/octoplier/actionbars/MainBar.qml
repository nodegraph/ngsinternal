import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.appwidgets 1.0

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

    // Internal Properties.
    property var last_mode: app_settings.node_graph_mode

    Component.onCompleted: {
    }
    
    // Methods.
    // Set the current mode of the action bar.
    function on_switch_to_mode(m) {
        clear_lit_buttons()
        if (m == app_settings.file_mode) {
            file_button.lit = true
            action_bar_title.text = "Node Files"
            more_menu_button.visible = true;
        } else if (m == app_settings.node_graph_mode) {
            node_graph_button.lit = true
            action_bar_title.text = "Node Graph"
            more_menu_button.visible = true;
        } else if (m == app_settings.view_node_mode) {
            view_node_button.lit = true
            action_bar_title.text = "Node Outputs"
            more_menu_button.visible = true;
        } else if (m == app_settings.edit_node_mode) {
            edit_node_button.lit = true
            action_bar_title.text = "Node Parameters"
            more_menu_button.visible = true;
        } else if (m == app_settings.posts_mode) {
            posts_button.lit = true
            action_bar_title.text = "Node Posts"
            more_menu_button.visible = false;
        } else if (m == app_settings.settings_mode) {
            settings_button.lit = true
            action_bar_title.text = "Settings"
            more_menu_button.visible = false;
        }
        switch_to_mode(m)
        last_mode = m
    }

    function switch_to_last_mode(m) {
        on_switch_to_mode(last_mode)
    }

    function clear_lit_buttons() {
        file_button.lit = false
        node_graph_button.lit = false
        view_node_button.lit = false
        edit_node_button.lit = false
        posts_button.lit = false
        settings_button.lit = false
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
        font.bold: true
        color: "white"
        anchors.left: parent.left
        anchors.leftMargin: app_settings.action_bar_left_margin
        anchors.verticalCenter: parent.verticalCenter
    }
    
    AppImageButton {
        id: more_menu_button
        
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: file_button.left
        
        image_url: "qrc:///icons/ic_more_vert_white_48dp.png"
        tooltip_text: "More Options"
        
        onClicked: {
            open_more_options();
        }
    }

    // File Mode Button.
    AppImageButton {
        id: file_button

        anchors.verticalCenter: parent.verticalCenter
        anchors.right: node_graph_button.left

        image_url: "qrc:///icons/ic_folder_open_white_48dp.png"
        tooltip_text: "Node Files"

        onClicked: {
            on_switch_to_mode(app_settings.file_mode)
        }
    }

    // Node Graph Mode Button.
    AppImageButton {
        id: node_graph_button
        
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: view_node_button.left
        
        image_url: "qrc:///icons/ic_widgets_white_48dp.png"
        tooltip_text: "Node Graph"
        
        onClicked: {
            on_switch_to_mode(app_settings.node_graph_mode)
        }
    }

    // View Node Mode Button.
    AppImageButton {
        id: view_node_button
        
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: edit_node_button.left
        
        image_url: "qrc:///icons/ic_looks_3_white_48dp.png"
        tooltip_text: "Node Outputs"
        
        onClicked: {
            on_switch_to_mode(app_settings.view_node_mode)
        }
    }
    
    // Edit Node Mode Button.
    AppImageButton {
        id: edit_node_button
        
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: posts_button.left
        
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        tooltip_text: "Node Parameters"
        
        onClicked: {
            on_switch_to_mode(app_settings.edit_node_mode)
        }
    }

    // Hot Posts Mode Button.
    AppImageButton {
        id: posts_button
        
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: settings_button.left
        
        image_url: "qrc:///icons/ic_whatshot_white_48dp.png"
        tooltip_text: "Node Posts"
        
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



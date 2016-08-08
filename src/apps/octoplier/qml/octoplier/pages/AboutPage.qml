import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import octoplier.stackedpages 1.0
import octoplier.appwidgets 1.0

Rectangle {
    id: info_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width
    
    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    // Appearance.
    color: app_settings.menu_stack_bg_color
    
    // Methods.
    function on_switch_to_mode(mode) {
    	if (mode == app_settings.about_mode) {
            visible = true;
        } else {
        	visible = false;
        }
    }

    // Logo.
    Image {
        id: about_image
        y: app_settings.page_height * 3/10
        width: app_settings.page_width / 3
        height: width

        anchors {
            horizontalCenter:  parent.horizontalCenter
        }

        source: "qrc:///images/octopus_white.png"
    }

    AppLabel {
        y: app_settings.page_height * 6/10
        text: "Smash Browse"
        anchors {
            horizontalCenter:  parent.horizontalCenter
        }
    }

    AppLabel {
        y: app_settings.page_height * 7/10
        text: "Beta Version 1.17316"
        anchors {
            horizontalCenter:  parent.horizontalCenter
        }
    }


}

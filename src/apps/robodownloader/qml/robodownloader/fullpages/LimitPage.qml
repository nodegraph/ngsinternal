import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import robodownloader.stackedmodepages 1.0
import robodownloader.appwidgets 1.0

Rectangle {
    id: info_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width
    
    // Positioning.
    x: 0
    y: 0
    z: 0

    // Appearance.
    color: app_settings.menu_stack_bg_color

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        title_text: "Feature Unavailable"
        show_back_button: true
        stack_view: info_page.Stack.view
    }

    AppLabel {
        y: app_settings.page_height * 2/10
        text: "Robo Downloader Lite " + node_graph_item.get_ngs_version()
        anchors {
            horizontalCenter:  parent.horizontalCenter
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

        source: "qrc:///images/robot_white.png"
    }

    AppLabel {
        y: app_settings.page_height * 6.5/10
        text: "Your edition permits a maximum of 10 nodes,"
        anchors {
            horizontalCenter:  parent.horizontalCenter
        }
    }

    AppLabel {
        y: app_settings.page_height * 7/10
        text: "and disables group node related features."
        anchors {
            horizontalCenter:  parent.horizontalCenter
        }
    }

}

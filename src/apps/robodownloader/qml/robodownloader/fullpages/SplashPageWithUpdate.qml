import QtQuick 2.7
import QtQuick.Controls 1.5

import robodownloader.appconfig 1.0
import robodownloader.appwidgets 1.0

Rectangle {
    id: splash_page
    anchors.fill: parent
    visible: true

    // This is hard coded because the AppSettings has not been loaded yet.
    color: "#FF0288D1" // a darker blue than the node graph blue

    property string ngs_version: ""
    property string app_name: ""
    
    AppUnits {
        id: app_units
    }

    AppSettings {
        id: app_settings
    }
    
    AppTooltip {
        id: app_tooltip
        z: app_settings.app_tooltip_z
        visible: false
    }

    // Logo.
    Image {
        id: splash_image
        width: parent.width / 3
        height: width
        x: width
        y: parent.height * 3/10
        source: "qrc:///images/robot_white.png"
    }

    Label {
        font.pointSize: 25
        font.italic: false
        color: "white"

        y: parent.height * 6.5/10
        text: app_name
        anchors {
            horizontalCenter:  parent.horizontalCenter
        }
    }

    // Version Label.
    //Label {
    //    font.pointSize: 25
    //    font.italic: false
    //    color: "white"
	//
    //    y: parent.height * 6.5/10
    //    text: "Version " + ngs_version
    //    anchors {
    //        horizontalCenter:  parent.horizontalCenter
    //    }
    //}
    
        Label {
        font.pointSize: 25
        font.italic: false
        color: "white"

        y: parent.height * 7/10
        text: "Updates Available"
        anchors {
            horizontalCenter:  parent.horizontalCenter
        }
    }
    
    AppLabelButton {
        id: update_app_button
        y: parent.height * 8/10
        anchors.horizontalCenter: parent.horizontalCenter
        text: "update app"
        onClicked: quick_view.start_app_update()
    }
    
    AppLabelButton {
        id: continue_button
        y: parent.height * 9/10
        anchors.horizontalCenter: parent.horizontalCenter
        text: "skip updates"
        onClicked: quick_view.close_splash_page()
    }

}

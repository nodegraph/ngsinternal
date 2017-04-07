import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4

// --------------------------------------------
// Splitter and Progress Bar.
// --------------------------------------------

Rectangle {
    id: splitter
    color: "#FFFFFFFF"
    property alias progress_bar: our_progress_bar

    // Dimensions.
    height: app_settings.splitter_height
    width: app_settings.page_width

    // Positioning.
    x: app_settings.splitter_x
    y: app_settings.splitter_y
    z: app_settings.splitter_z
    
    function start() {
    	progress_timer.start()
    	our_progress_bar.visible = true
    }
    
    function stop() {
    	progress_timer.stop()
    	our_progress_bar.visible = false
    }
    
    Timer {
        id: progress_timer
        interval: 66
        running: false
        repeat: true
        onTriggered: {
        	if (manipulator.is_busy_cleaning()) {
	        	if (our_progress_bar.value >= 100) { 
	        		our_progress_bar.value = 0
	        	} else {
	        		our_progress_bar.value += 1
	        	}
        	} else {
        		our_progress_bar.value = 0
        	}
        }
    }

    // URL loading progress overlay.
    // Note that the progress bar won't show anything on ios and winrt platforms.
    // This is due to way webview's loading and loadProgress are currently imlemented.
    // This is as of Qt 5.6. See the Qt minibrowser example for more details.
    ProgressBar {
        id: our_progress_bar
        anchors.fill: parent
        z: 1
        visible: false
        minimumValue: 0
        maximumValue: 100
        value: 0

        style: ProgressBarStyle {
            background: Rectangle {
                color: "transparent"
            }
            progress: Rectangle {
                anchors.fill: parent
                color: "#FFFF0000"
            }
        }
    }
    
    Component.onCompleted: {
        start()
    }
}


import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import robodownloader.appconfig 1.0
import robodownloader.stackedmodepages 1.0
import robodownloader.fullpages 1.0
import robodownloader.appwidgets 1.0
import robodownloader.contentpages.listmodels 1.0


BaseListDelegate {
    id: delegate
    
    // Displayed Content.
    property var _letters: ""
    
    icon_component: Component {
    	id: icon_component
    	Rectangle {
    		color: "transparent"
			Text {
	            //y: app_settings.list_item_height_large / 12
	            horizontalAlignment: Text.AlignHCenter
	            verticalAlignment:  Text.AlignVCenter
	            clip: true
	            
	            height: parent.height
            	width: parent.height
            	anchors.verticalCenter: parent.verticalCenter
            	anchors.horizontalCenter: parent.horizontalCenter
	            
	            font.family: "Arial"
	            font.bold: true
			    font.pointSize: 1.7 * app_settings.large_font_point_size
			    color: "white"
	            text: delegate._letters
	        }
        }
    }
}
import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import smashbrowse.appconfig 1.0
import smashbrowse.stackedmodepages 1.0
import smashbrowse.fullpages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.contentpages.listmodels 1.0


BaseListDelegate {
    id: delegate
    
    // Displayed Content.
    property var _image_url: ""
    
    icon_component: Component {
    	id: icon_component
    	Rectangle {
    		color: "transparent"
	    	Image {
	    		source: delegate._image_url
	    		
	            height: parent.height * 0.75
            	width: parent.height * 0.75
            	anchors.verticalCenter: parent.verticalCenter
            	anchors.horizontalCenter: parent.horizontalCenter
	        }
        }
    }
}
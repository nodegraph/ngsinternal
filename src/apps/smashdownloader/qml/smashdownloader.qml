import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
//import Qt.labs.settings 1.0

//import NodeGraphRendering 1.0
import smashbrowse.fullpages 1.0
import smashbrowse.modepages 1.0
import smashbrowse.stackedmodepages 1.0
import smashbrowse.contentpages.listpages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.appconfig 1.0
import smashbrowse.contentpages.listmodels 1.0
import smashbrowse.contentpages.enterdatapages 1.0

import pages 1.0

Rectangle {
    id: app_window
    anchors.fill: parent
    visible: true
    color: "blue"
    
    property string version_number: ""

    // Clean up routine.
    function on_closing(close) {
    }

    // Global App Objects.

    AppEnums {
        id: app_enums
    }

    AppUnits {
        id: app_units
    }

    AppSettings {
        id: app_settings
    }

    AppUtils {
        id: app_utils
    }

    AppLoader {
        id: app_loader
    }

    AppTooltip {
        id: app_tooltip
        z: app_settings.app_tooltip_z
        visible: false
    }

    // Pages.
    DownloadListPage {
    	id: downloads_page
        visible: true
        x: 0
	    y: 0
	    z: 0
        height: app_settings.screen_height
    	width: app_settings.screen_width
    	
    	Component {
	   		id: settings_button
	   		AppImageButton {
	   			x: app_settings.page_content_right - app_settings.image_button_width
		        anchors {
		        	verticalCenter: parent.verticalCenter
		        }
		        
		        image_url: "qrc:///icons/ic_settings_white_48dp.png"
		        tooltip_text: "Settings"
		        
		        onClicked: {
		            downloads_page.visible = false
		            
		            download_settings.version_number = version_number
		            download_settings.update_fields()
		            download_settings.visible = true
		        }
		    }
		}
		
		Component {
	   		id: downloaded_button
	   		AppImageButton {
	   			x: app_settings.page_content_right - 2*app_settings.image_button_width - app_settings.page_right_margin
		        anchors {
		        	verticalCenter: parent.verticalCenter
		        }
		        
		        image_url: "qrc:///icons/ic_music_video_white_48dp.png"
		        tooltip_text: "Settings"
		        
		        onClicked: {
		            downloads_page.visible = false
		            downloaded_page.visible = true
		        }
		    }
		}
    	
        Component.onCompleted: {
        	stack_view_header.title_text = "double click to cancel"
        	stack_view_header.show_back_button = false
        	settings_button.createObject(stack_view_header, {});
        	downloaded_button.createObject(stack_view_header, {});
    	}
    }
    
    // Pages.
    DownloadedListPage {
    	id: downloaded_page
        visible: false
        x: 0
	    y: 0
	    z: 0
        height: app_settings.screen_height
    	width: app_settings.screen_width
    	
    	Component {
	   		id: settings_button2
	   		AppImageButton {
	   			x: app_settings.page_content_right - app_settings.image_button_width
		        anchors {
		        	verticalCenter: parent.verticalCenter
		        }
		        
		        image_url: "qrc:///icons/ic_settings_white_48dp.png"
		        tooltip_text: "Settings"
		        
		        onClicked: {
		            downloaded_page.visible = false
		            
		            download_settings.version_number = version_number
		            download_settings.update_fields()
		            download_settings.visible = true
		        }
		    }
		}
		
		Component {
	   		id: downloading_button
	   		AppImageButton {
	   			x: app_settings.page_content_right - 2*app_settings.image_button_width - app_settings.page_right_margin
		        anchors {
		        	verticalCenter: parent.verticalCenter
		        }
		        
		        image_url: "qrc:///icons/ic_file_download_white_48dp.png"
		        tooltip_text: "Settings"
		        
		        onClicked: {
		            downloaded_page.visible = false
		            downloads_page.visible = true
		        }
		    }
		}
    	
        Component.onCompleted: {
        	stack_view_header.title_text = "double click to reveal file"
        	stack_view_header.show_back_button = false
        	settings_button2.createObject(stack_view_header, {});
        	downloading_button.createObject(stack_view_header, {});
    	}
    }

    DownloadSettingsPage {
        id: download_settings
        visible: false
    }
    
    // Stuff to do when this component is completed.
    Component.onCompleted: {
        update_dependencies()
    }

	function on_license_checked(valid) {
		console.log("license is valid: " + license_checker.license_is_valid())
    	console.log("loading stuff")
    	file_model.load_model()
    	file_model.load_graph()
	}

    function update_dependencies() {
        // Download manager connections.
        download_manager.download_queued.connect(downloads_page.on_download_queued)
        download_manager.download_started.connect(downloads_page.on_download_started)
        download_manager.download_progress.connect(downloads_page.on_download_progress)
        download_manager.download_finished.connect(downloads_page.on_download_finished)
        download_manager.download_errored.connect(downloads_page.on_download_errored)

        // Hook up some pre-close tasks.
        quick_view.closing.connect(app_window.on_closing)
        
        // Disable encryption.
        crypto_logic.enable_encryption(false)
        
        // Load our license.
        license_checker.load()
        app_utils.check_license(on_license_checked)
        console.log("www waiting for license check")
    }
}

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import smashbrowse.appwidgets 1.0

Rectangle {
    id: page

    // Dimensions.
    height: app_settings.screen_height
    width: app_settings.screen_width

    // Positioning.
    x: 0
    y: 0
    z: 10

    // Properties.
    color: app_settings.menu_stack_bg_color

    // Internal data.
    property string license_cache: ""
    property string edition_cache: ""
    property string version_number: ""

    function update_fields() {
		load_license()
		load_settings()
    }
    
    function load_license() {
    	// Clear the license processing status text.
    	status_label.text = ""
    	
    	// Load the license.
        license_checker.load()
        
        // Update the gui fields.
        license_text_field.text = license_checker.get_license()
        if (license_checker.get_edition() == "pro") {
            pro_edition_button.checked = true
            lite_edition_button.checked = false
        } else {
            pro_edition_button.checked = false
            lite_edition_button.checked = true
        }
    }
    
    function load_settings() {
    	// Make sure the model is loaded.
    	file_model.load_model()
    	file_model.load_graph()
    	// We currently only have one value.
    	max_concurrent_downloads_text_field.text = file_model.get_work_setting('max_concurrent_downloads')
    	default_downloads_dir_text_field.text = file_model.get_work_setting('default_downloads_directory')
    }
    
    function save_settings() {
        // We currently only have one value.
        var info = {}
        info.max_concurrent_downloads = Number(max_concurrent_downloads_text_field.text)
        info.default_downloads_directory = default_downloads_dir_text_field.text
        
        var row = file_model.get_working_row()
        file_model.update_graph(row, info)
    }

    function on_license_checked(valid) {
    	if (!valid) {
    		status_label.text = "license is invalid"
    		return
    	}
        // Record the license.
        license_checker.save()
        // Save the settings.
        save_settings()
        // Hide this page.
        page.visible = false
        // Erase passwords from page.
        license_text_field.text = ""
        license_cache = ""
        // Go back to the downloads page.
        downloads_page.visible = true
    }
    
    FileDialog {
	    id: file_dialog
	    title: "Please choose a folder"
	    folder: shortcuts.home
	    selectFolder: true
	    onAccepted: {
	    	var dir = file_dialog.folder.toString()
        	dir = dir.replace(/^(file:\/{3})/,"");
        	// unescape html codes
        	default_downloads_dir_text_field.text = decodeURIComponent(dir);
	    }
	    onRejected: {
	    }
	    //Component.onCompleted: visible = true
	}

    ColumnLayout {
        height: app_settings.screen_height
        width: app_settings.screen_width
        spacing: app_settings.column_layout_spacing * 2

        // Filler.
        Item {Layout.fillHeight: true}
        
        // License lablel.
        Label {
            id: version_label
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Smash Downloader Version"
            font.pointSize: app_settings.large_font_point_size
            font.bold: false
            font.italic: false
            color: "white"
        }
        
        //Label {
        //    id: version_number_label
        //    anchors.horizontalCenter: parent.horizontalCenter
        //    text: version_number
        //    font.pointSize: app_settings.large_font_point_size
        //    font.bold: false
        //    font.italic: false
        //    color: "white"
        //}
        
        // License field.
        AppLineEdit {
            id: version_number_field
            anchors.horizontalCenter: parent.horizontalCenter
            text: version_number
            readOnly: true
        }
        
        // License lablel.
        Label {
            id: app_label
            anchors.horizontalCenter: parent.horizontalCenter
            text: "License Key"
            font.pointSize: app_settings.large_font_point_size
            font.bold: false
            font.italic: false
            color: "white"
        }

        // License field.
        AppLineEdit {
            id: license_text_field
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: app_settings.page_left_margin
                rightMargin: app_settings.page_right_margin
            }
            text: ""
        }

        RowLayout {
                anchors.horizontalCenter: parent.horizontalCenter
                ExclusiveGroup { id: edition_group }
                AppRadioButton {
                	id: pro_edition_button
                    text: "Pro"
                    checked: true
                    exclusiveGroup: edition_group
                }
                Rectangle {
                    height: app_settings.check_box_size
                    width: 2 * app_settings.check_box_size
                    color: 'transparent'
                }
                AppRadioButton {
                	id: lite_edition_button
                    text: "Lite"
                    checked: false
                    exclusiveGroup: edition_group
                }
            }
            
        AppLabel {
            id: max_concurrent_downloads_label
            text: "Max Concurrent Downloads"
            anchors {
            	horizontalCenter: parent.horizontalCenter
                leftMargin: app_settings.page_left_margin
                rightMargin: app_settings.page_right_margin
            }
        }

        // Title Field.
        AppLineEdit {
            id: max_concurrent_downloads_text_field
            enabled: pro_edition_button.checked
            text: '2'
            validator: RegExpValidator{regExp: /\d+/}
            inputMethodHints: Qt.ImhDigitsOnly
            anchors {
            	horizontalCenter: parent.horizontalCenter
                leftMargin: app_settings.page_left_margin
                rightMargin: app_settings.page_right_margin
            }
        }
        
        AppLabel {
            id: default_download_dir_label
            text: "Default Download Directory"
            anchors {
            	horizontalCenter: parent.horizontalCenter
                leftMargin: app_settings.page_left_margin
                rightMargin: app_settings.page_right_margin
            }
        }
        
        RowLayout {
        	Item {
        		Layout.fillWidth: true
        	}
        		
	        AppLineEdit {
	            id: default_downloads_dir_text_field
	            enabled: pro_edition_button.checked
	            text: ''
	            validator: RegExpValidator{regExp: /\d+/}
	            inputMethodHints: Qt.ImhDigitsOnly
	            anchors {
	                leftMargin: app_settings.page_left_margin
	                rightMargin: app_settings.page_right_margin
	            }
	        }
	        
	        // Continue button.
	        AppLabelButton {
	            id: file_dialog_button
	            enabled: pro_edition_button.checked
	            text: "..."
	            onClicked: {
	            	if (default_downloads_dir_text_field.text != "") {
	            		file_dialog.folder = "file:///" + default_downloads_dir_text_field.text
	            	}
	            	file_dialog.open()
	            }
	        }
	        
	        Item {
        		Layout.fillWidth: true
        	}
        }

        // Continue button.
        AppLabelButton {
            id: continue_button
            anchors.horizontalCenter: parent.horizontalCenter
            text: "continue"
            onClicked: {
                license_cache = license_text_field.text
                if (pro_edition_button.checked) {
                	edition_cache = "pro"
                } else {
                	edition_cache = "lite"
                }
                license_checker.set_edition(edition_cache)
                license_checker.set_license(license_cache)
                app_utils.check_license(on_license_checked)
            }
        }

        // Shows status of license processing.
        Label {
            id: status_label
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
            font.pointSize: app_settings.font_point_size
            font.bold: false
            font.italic: false
            color: "yellow"

            function on_mouse_pressed() {
                text = "processing ..."
                update()
            }
        }

        // Filler.
        Item {Layout.fillHeight: true}

        // Hook up our signals.
        Component.onCompleted: {
            continue_button.mouse_pressed.connect(status_label.on_mouse_pressed)
        }
    }
}



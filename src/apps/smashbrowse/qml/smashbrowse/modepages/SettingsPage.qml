import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import smashbrowse.stackedmodepages 1.0
import smashbrowse.appwidgets 1.0

Rectangle {
    id: settings_page

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
        if (mode == app_settings.settings_mode) {
            if (file_model.get_working_row()>=0) {
                stack_view_header.title_text = file_model.get_work_setting('title') + ' file'
                lock_links_check_box.checked = file_model.get_work_setting('lock_links')
                max_concurrent_downloads_text_field.text = file_model.get_work_setting('max_concurrent_downloads')
                max_node_posts_text_field.text = file_model.get_work_setting('max_node_posts')
                auto_run_check_box.checked = file_model.get_work_setting('auto_run')
                auto_run_interval_text_field.text = file_model.get_work_setting('auto_run_interval')
                default_downloads_dir_text_field.text = file_model.get_work_setting('default_downloads_directory')
            }
            visible = true;
        } else {
            visible = false;
        }
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

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        show_back_button: true
    }

    ColumnLayout {
        // Geometry.
        height: app_settings.menu_page_height
        width: app_settings.menu_page_width

        // Positioning.
        x: 0
        y: app_settings.action_bar_height
        z: 0

        // Appearance.
        spacing: app_settings.column_layout_spacing

        RowLayout {
            AppLabel {
                id: max_concurrent_downloads_label
                enabled: license_checker.has_valid_pro_license
	        	opacity: license_checker.has_valid_pro_license ? 1.0 : 0.5
                text: "Max Concurrent Downloads"
                anchors {
                    left: parent.left
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }

            // Title Field.
            AppLineEdit {
                id: max_concurrent_downloads_text_field
                enabled: license_checker.has_valid_pro_license
	        	opacity: license_checker.has_valid_pro_license ? 1.0 : 0.5
                text: '1'
                tool_bar: copy_paste_bar
                validator: RegExpValidator{regExp: /\d+/}
                inputMethodHints: Qt.ImhDigitsOnly
                anchors {
                    left: max_concurrent_downloads_label.right
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }
        }
        
        AppLabel {
            id: default_download_dir_label
            enabled: license_checker.has_valid_pro_license
	        opacity: license_checker.has_valid_pro_license ? 1.0 : 0.5
            text: "Default Download Directory"
            anchors {
            	left: parent.left
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
	            enabled: license_checker.has_valid_pro_license
	            opacity: license_checker.has_valid_pro_license ? 1.0 : 0.5
	            text: ''
	            validator: RegExpValidator{regExp: /\d+/}
	            inputMethodHints: Qt.ImhDigitsOnly
	            anchors {
	            	left: parent.left
                	right: file_dialog_button.left
	                leftMargin: app_settings.page_left_margin
	                rightMargin: app_settings.page_right_margin
	            }
	        }
	        
	        // File dialog button.
	        AppLabelButton {
	            id: file_dialog_button
	            enabled: license_checker.has_valid_pro_license
	            opacity: license_checker.has_valid_pro_license ? 1.0 : 0.5
	            text: "..."
	            onClicked: {
	            	if (default_downloads_dir_text_field.text != "") {
	            		file_dialog.folder = "file:///" + default_downloads_dir_text_field.text
	            	}
	            	file_dialog.open()
	            }
	            anchors {
	            	right: parent.right
	                leftMargin: app_settings.page_left_margin
	                rightMargin: app_settings.page_right_margin
	            }
	        }
	        
	        Item {
        		Layout.fillWidth: true
        	}
        }
        
        RowLayout {
            AppLabel {
                id: lock_links_label
                text: "Lock Links"
                anchors {
                    left: parent.left
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }

            AppCheckBox {
                id: lock_links_check_box
                checked: false
                anchors {
                    left: lock_links_label.right
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }
        }

        RowLayout {
            AppLabel {
                id: max_node_posts_label
                text: "Max Node Posts"
                anchors {
                    left: parent.left
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }

            // Title Field.
            AppLineEdit {
                id: max_node_posts_text_field
                text: '1000'
                tool_bar: copy_paste_bar
                validator: RegExpValidator{regExp: /\d+/}
                inputMethodHints: Qt.ImhDigitsOnly
                anchors {
                    left: max_node_posts_label.right
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }
        }

        RowLayout {
            AppLabel {
                id: auto_run_label
                text: "Auto Run"
                anchors {
                    left: parent.left
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }

            AppCheckBox {
                id: auto_run_check_box
                checked: false
                anchors {
                    left: auto_run_label.right
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }
        }

        RowLayout {
            AppLabel {
                id: auto_run_interval_label
                text: "Auto Run Interval (seconds)"
                anchors {
                    left: parent.left
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }

            // Title Field.
            AppLineEdit {
                id: auto_run_interval_text_field
                tool_bar: copy_paste_bar
                text: '60'
                validator: RegExpValidator{regExp: /\d+/}
                inputMethodHints: Qt.ImhDigitsOnly
                anchors {
                    left: auto_run_interval_label.right
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }
        }

        // Buttons.
        RowLayout {
        	Layout.maximumWidth: parent.width
        	
            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "accept"
                onClicked: {
                    // Gather values.
                    var info = {}
                    info.auto_run = auto_run_check_box.checked
                    info.auto_run_interval = Number(auto_run_interval_text_field.text)
                    info.lock_links = lock_links_check_box.checked
                    info.max_node_posts = Number(max_node_posts_text_field.text)
                    info.max_concurrent_downloads = Number(max_concurrent_downloads_text_field.text)
                    info.default_downloads_directory = default_downloads_dir_text_field.text

                    // Set values.
                    var row = file_model.get_working_row()
                    file_model.update_graph(row, info)

                    // Pop stack down to the file page.
                    main_bar.switch_to_last_mode()
                }
            }
            Rectangle {
                color: "transparent"
                height: app_settings.action_bar_height
                width: app_settings.button_spacing
            }
            AppLabelButton {
                text: "cancel"
                onClicked: {
                    main_bar.switch_to_last_mode()
                }
            }
            Item {Layout.fillWidth: true}
        }

    }


}

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import smashbrowse.appwidgets 1.0

Rectangle {
    id: check_license_page

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

    function update_fields() {
        // Since this page is built before the file_model is ready, we need to set the fields manually.
        license_text_field.text = file_model.get_license()
        if (file_model.get_edition() == "pro") {
            pro_edition_button.checked = true
            lite_edition_button.checked = false
        } else {
            pro_edition_button.checked = false
            lite_edition_button.checked = true
        }
    }

    function on_license_checked(valid) {
    	if (!valid) {
    		status_label.text = "license is invalid"
    		return
    	}
        // Record the license.
        file_model.set_license(edition_cache, license_cache)
        file_model.save_crypto()
        // Hide this page.
        check_license_page.visible = false
        // Erase passwords from page.
        license_text_field.text = ""
        license_cache = ""
        // Switch to node graph mode.
        main_bar.on_switch_to_mode(app_settings.node_graph_mode)
        app_worker.make_browser_visible()
        // Load the last graph.
        app_utils.load_last_graph()
    }

    ColumnLayout {
        height: app_settings.screen_height
        width: app_settings.screen_width
        spacing: app_settings.column_layout_spacing * 2

        // Filler.
        Item {Layout.fillHeight: true}

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
        AppTextField {
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
                app_utils.check_license(edition_cache, license_cache, on_license_checked)
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



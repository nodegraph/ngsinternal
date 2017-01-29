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
                stack_view_header.title_text = file_model.get_work_setting('title')
                lock_links_check_box.checked = file_model.get_work_setting('lock_links')
                max_concurrent_downloads_text_field.text = file_model.get_work_setting('max_concurrent_downloads')
                max_node_posts_text_field.text = file_model.get_work_setting('max_node_posts')
                auto_run_check_box.checked = file_model.get_work_setting('auto_run')
                auto_run_interval_text_field.text = file_model.get_work_setting('auto_run_interval')
            }
            visible = true;
        } else {
            visible = false;
        }
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
                id: max_concurrent_downloads_label
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
                text: '2'
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
                text: "Auto Run Interval (mins)"
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

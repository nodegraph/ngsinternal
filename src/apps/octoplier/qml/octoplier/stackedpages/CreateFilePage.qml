import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.appwidgets 1.0

Rectangle {
    id: create_file_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Properties.
    color: app_settings.menu_stack_bg_color

    // This is set when added to the app stack view.
    property var parent_stack_view

    // Properties.
    property bool create_file: true
    property alias title_field: title_field
    property alias description_field: description_field

    // Our Methods.
    function set_title(title) {
        stack_view_header.title_text = title
    }

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        stack_view: parent_stack_view
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


        ColumnLayout {
            spacing: 0
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: app_settings.page_left_margin
                rightMargin: app_settings.page_right_margin
            }

            AppLabel {
                id : title_label
                text: "Title"
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }

            // Title Field.
            AppTextField {
                id: title_field
                tool_bar: copy_paste_bar
                text: create_file ? file_model.get_default_info()['title'] : file_stack_page.get_file_page().get_current_title()
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }
        }

        ColumnLayout {
            spacing: 0
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: app_settings.page_left_margin
                rightMargin: app_settings.page_right_margin
            }
            AppLabel {
                text: "Description"
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }

            // Description Field.
            AppTextField {
                id: description_field
                tool_bar: copy_paste_bar
                text: create_file ? file_model.get_default_info()['description'] : file_stack_page.get_file_page().get_current_description()
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }
        }

        RowLayout {
            AppLabel {
                id: hide_passwords_label
                text: "Hide Passwords"
                anchors {
                    left: parent.left
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }
            AppCheckBox {
                id: hide_passwords_check_box
                checked: create_file ? file_model.get_default_info()['hide_passwords'] : file_stack_page.get_file_page().get_current_hide_passwords()
                anchors {
                    left: hide_passwords_label.right
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
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
                checked: create_file ? file_model.get_default_info()['lock_links'] : file_stack_page.get_file_page().get_current_lock_links()
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
            AppTextField {
                id: max_node_posts_text_field
                tool_bar: copy_paste_bar
                text: create_file ? file_model.get_default_info()['max_node_posts'] : file_stack_page.get_file_page().get_current_max_node_posts()
                inputMethodHints: Qt.ImhFormattedNumbersOnly
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
                checked: create_file ? file_model.get_default_info()['auto_run'] : file_stack_page.get_file_page().get_current_auto_run()
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
            AppTextField {
                id: auto_run_interval_text_field
                tool_bar: copy_paste_bar
                text: create_file ? file_model.get_default_info()['auto_run_interval'] : file_stack_page.get_file_page().get_current_auto_run_interval()
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                anchors {
                    left: auto_run_interval_label.right
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
            }
        }

        // Buttons.
        RowLayout {
            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "accept"
                onClicked: {
                    // Gather values.
                    var info = {}
                    info.title = title_field.text
                    info.description = description_field.text
                    info.auto_run = auto_run_check_box.checked
                    info.auto_run_interval = Number(auto_run_interval_text_field.text)
                    info.hide_passwords = hide_passwords_check_box.checked
                    info.lock_links = lock_links_check_box.checked
                    info.max_node_posts = Number(max_node_posts_text_field.text)

                    // Set values.
                    if (create_file == true) {
                        file_model.create_graph(info)
                        node_graph_page.node_graph.update()
                    } else {
                        file_stack_page.get_file_page().update_current_graph(info)
                    }

                    // Pop stack down to the file page.
                    file_stack_page.on_close_file_options()
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
                    create_file_page.parent_stack_view.pop_page()
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}



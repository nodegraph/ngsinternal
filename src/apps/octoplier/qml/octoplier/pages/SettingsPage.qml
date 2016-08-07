import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0
import Qt.labs.settings 1.0

import octoplier.stackedpages 1.0
import octoplier.appwidgets 1.0

// todo: the settings page should have the following
// 1) lock links by default
// 2) hide password characters
// 3) time interval to run scripts
// 4) max number of posts to accumulate


// Also it seems we have a bug where there are ghost components in the node graph.
// This can be noticed when doing a frame all on one or a fest nodes.
// It looks as though it's framing some ghost item as well.

Rectangle {
    id: page

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
            visible = true;
        } else {
        	visible = false;
        }
    }

    ColumnLayout {
        y: app_settings.action_bar_height

        //height: app_settings.menu_page_height
        width: app_settings.menu_page_width
        spacing: app_settings.column_layout_spacing

        Rectangle {
            height: app_settings.column_layout_spacing
            width: app_settings.menu_page_width
            color: "transparent"
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
                checked: settings.hide_passwords
                anchors {
                    left: hide_passwords_label.right
                    leftMargin: app_settings.page_left_margin
                    rightMargin: app_settings.page_right_margin
                }
                onClicked: {
                    console.log('checkbox checked: ' + checked)
                }
            }
        }

        AppLabel {
            text: "Lock Links"
        }

        CheckBox {
            id: lock_links_check_box
            checked: settings.lock_links
        }

        AppLabel {
            text: "max_node_posts"
        }

        // Title Field.
        AppTextField {
            id: max_node_posts_text_field
            tool_bar: copy_paste_bar
            text: "untitled"
            anchors {
		        left: parent.left
		        right: parent.right
		        leftMargin: app_settings.page_left_margin
		        rightMargin: app_settings.page_right_margin
		    }
        }

        // Buttons.
        RowLayout {
            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "accept"
                onClicked: {
                    if (mode == "create") {
                        var unique_title = file_model.make_title_unique(title_field.text)
                        file_model.create_graph(unique_title, description_field.text)
                        node_graph_page.node_graph.update()
                    } else {
                        file_stack_page.update_current_graph(title_field.text, description_field.text)
                    }
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

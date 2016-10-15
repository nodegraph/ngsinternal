import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import smashbrowse.appwidgets 1.0

Rectangle {
    id: edit_string_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Appearance.
    color: app_settings.menu_stack_bg_color
    
    // Our Methods.
    function set_value(value) {
        text_field.text = value
    }

    function get_value(value) {
        return text_field.text
    }

    function set_title(title) {
        stack_view_header.title_text = title
    }
    function get_title() {
        return stack_view_header.title_text
    }

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        stack_view: edit_string_page.Stack.view
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

        // Title Field.
        AppTextField {
            id: text_field
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
                    var path = edit_string_page.Stack.view.get_title_path(1, edit_string_page.Stack.view.depth)
                    edit_string_page.Stack.view._stack_page.set_value(path, get_value())
                    edit_string_page.Stack.view.pop_page()
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
                    edit_string_page.Stack.view.pop_page()
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}



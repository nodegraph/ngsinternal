import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import smashbrowse.appwidgets 1.0

Rectangle {
    id: enter_url_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Appearance.
    color: app_settings.menu_stack_bg_color

    // This is set when added to the app stack view.
    property var parent_stack_view
    property var value

    // Methods.
    function init(value) {
        text_field.text = value
        enter_url_page.value = value
    }

    // Our Methods.
    function set_title(title) {
        stack_view_header.title_text = title
    }
    function get_title() {
        return stack_view_header.title_text
    }

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        stack_view: parent_stack_view
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
                    enter_url_page.value = text_field.text
                    app_comm.navigate_to(text_field.text)
                    enter_url_page.parent_stack_view.pop_page()
                    main_bar.switch_to_current_mode()
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
                    enter_url_page.parent_stack_view.pop_page()
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}



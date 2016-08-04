import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.appwidgets 1.0

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
    color: app_settings.ng_bg_color
    // This is set when added to the app stack view.
    property var parent_stack_view
    property var value

    // Methods.
    function init(value) {
        text_field.text = value
        edit_string_page.value = value
    }

    ColumnLayout {
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
            onAccepted: {
            }
        }

        // Buttons.
        RowLayout {
            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "accept"
                onClicked: {
                    edit_string_page.value = text_field.text
                    edit_string_page.parent_stack_view.pop_page()
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
                    edit_string_page.parent_stack_view.pop_page()
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}



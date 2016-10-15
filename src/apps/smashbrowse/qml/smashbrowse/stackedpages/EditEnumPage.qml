import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import smashbrowse.appwidgets 1.0

Rectangle {
    id: edit_enum_page

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
    property string enum_type

    // Our Methods.
    function set_enum_type(type) {
        enum_type = type
        if (enum_type == 'MouseActionType') {
            check_box.set_options(app_enums.mouse_action_types)
        } else if (enum_type == 'TextActionType') {
            check_box.set_options(app_enums.text_action_types)
        } else if (enum_type == 'ElementActionType') {
            check_box.set_options(app_enums.element_action_types)
        } else if (enum_type == 'WrapType') {
            check_box.set_options(app_enums.wrap_types)
        } else if (enum_type == 'Direction') {
            check_box.set_options(app_enums.direction_types)
        } else {
            console.log('Error: attempt to edit an enum with invalid type.')
            console.log(new Error().stack);
        }
    }
    function get_enum_type() {
        return enum_type
    }

    // Note the enum type should be set before this.
    function set_value(value) {
        if (value >= combo_box.count || value < 0) {
            console.log('Error: attempt to set a value that is out of range for this enum type.')
            console.log(new Error().stack);
        }
        combo_box.currentIndex = value
    }
    function get_value(value) {
        return combo_box.currentIndex
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

        AppComboBox {
            id: combo_box
        }

        // Buttons.
        RowLayout {
            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "accept"
                onClicked: {
                    var path = parent_stack_view.get_title_path(1, parent_stack_view.depth)
                    parent_stack_view.stack_page.set_value(path, get_value())
                    parent_stack_view.pop_page()
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
                    parent_stack_view.pop_page()
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}



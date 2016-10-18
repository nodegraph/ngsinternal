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
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Appearance.
    color: app_settings.menu_stack_bg_color
    
    property string description: ""

    // Our Methods.
    function set_enum_type(type) {
        if (type == msg_enum_enum.kMouseActionType) {
            combo_box.set_option_texts(app_enums.mouse_action_type_text)
        } else if (type == msg_enum_enum.kTextActionType) {
            combo_box.set_option_texts(app_enums.text_action_type_text)
        } else if (type == msg_enum_enum.kElementActionType) {
            combo_box.set_option_texts(app_enums.element_action_type_text)
        } else if (type == msg_enum_enum.kWrapType) {
            combo_box.set_option_texts(app_enums.wrap_type_text)
        } else if (type == msg_enum_enum.kDirectionType) {
            combo_box.set_option_texts(app_enums.direction_type_text)
        } else {
            console.log('Error: attempt to edit an enum with invalid type.')
            console.log(new Error().stack);
        }
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
    
    function set_description_label(text) {
    	description_label.text = text
    }

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        stack_view: page.Stack.view
    }

    ColumnLayout {
        y: app_settings.action_bar_height

        //height: app_settings.menu_page_height
        width: app_settings.menu_page_width
        spacing: app_settings.column_layout_spacing

        Layout.maximumWidth: app_settings.menu_page_width

        Rectangle {
            height: app_settings.column_layout_spacing
            width: app_settings.menu_page_width
            color: "transparent"
        }
        
        Text  {
            id: description_label
            width: app_settings.page_width
            maximumLineCount: 5
            elide: Text.ElideLeft
            clip: true
            anchors {
                left: parent.left
                right: parent.right
                leftMargin: app_settings.page_left_margin
                rightMargin: app_settings.page_right_margin
            }
            Layout.maximumWidth: parent.width
	        
            text: description
            font.pointSize: app_settings.large_font_point_size
            font.bold: false
            font.italic: false
            color: "white"
            wrapMode: Text.WordWrap
        }
        
        Rectangle {
            height: app_settings.column_layout_spacing
            width: app_settings.menu_page_width
            color: "transparent"
        }

        AppComboBox {
            id: combo_box
        }
        
        Rectangle {
            height: app_settings.column_layout_spacing
            width: app_settings.menu_page_width
            color: "transparent"
        }

        // Buttons.
        RowLayout {
            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "accept"
                onClicked: {
                    var path = page.Stack.view.get_title_path(1, page.Stack.view.depth)
                    page.Stack.view._stack_page.set_value(path, get_value())
                    page.Stack.view.pop_page()
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
                    page.Stack.view.pop_page()
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}



import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import youmacro.appwidgets 1.0

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
        stack_view: create_file_page.Stack.view
    }

    ColumnLayout {
        // Geometry.
        height: app_settings.menu_page_height / 2
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
            AppLineEdit {
                id: title_field
                tool_bar: copy_paste_bar
                text: create_file ? file_model.get_default_settings().title : file_model.get_current_title()
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
            AppLineEdit {
                id: description_field
                tool_bar: copy_paste_bar
                text: create_file ? file_model.get_default_settings().description : file_model.get_current_description()
                anchors {
                    left: parent.left
                    right: parent.right
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
                    info.title = title_field.text
                    info.description = description_field.text

                    // Set values.
                    if (create_file == true) {
                    	file_menu_list_stack_page.on_create_graph_info(info)
                    } else {
                        file_menu_list_stack_page.on_update_current_graph_info(info)
                    }
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
                    create_file_page.Stack.view.pop_page()

                    console.log('default settings: ' + JSON.stringify(file_model.get_default_settings()))
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}



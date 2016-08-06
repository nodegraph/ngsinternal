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
    color: app_settings.ng_bg_color

    // This is set when added to the app stack view.
    property var parent_stack_view

    // Properties.
    property string mode: "create"
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
        y: app_settings.action_bar_height

        //height: app_settings.menu_page_height
        width: app_settings.menu_page_width
        spacing: app_settings.column_layout_spacing

        Rectangle {
            height: app_settings.column_layout_spacing
            width: app_settings.menu_page_width
            color: "transparent"
        }

        AppLabel {
            text: "Title"
        }

        // Title Field.
        AppTextField {
            id: title_field
            tool_bar: copy_paste_bar
            text: "untitled"
            onAccepted: {
            }
        }

        AppLabel {
            text: "Description"
        }

        // Description Field.
        AppTextField {
            id: description_field
            tool_bar: copy_paste_bar
            text: "This does something."
            onAccepted: {
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



import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import smashbrowse.appwidgets 1.0

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
            AppTextField {
                id: title_field
                tool_bar: copy_paste_bar
                text: create_file ? file_model.get_default_settings().title : file_stack_page.get_file_page().get_current_setting('title') //
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
                text: create_file ? file_model.get_default_settings().description : file_stack_page.get_file_page().get_current_setting('description') //
                anchors {
                    left: parent.left
                    right: parent.right
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

                    // Set values.
                    if (create_file == true) {
                        file_model.create_graph(info)
                        node_graph_item.update()
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

                    console.log('default settings: ' + JSON.stringify(file_model.get_default_settings()))
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}



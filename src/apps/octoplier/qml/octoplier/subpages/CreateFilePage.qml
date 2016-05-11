import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.tools 1.0

Rectangle {
    id: create_file_page

    // Dimensions.
    height: app_settings.menu_page_height
    width: app_settings.menu_page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Properties.
    color: app_settings.ng_bg_color

    property string mode: "create"
    property alias title_field: title_field
    property alias description_field: description_field

    Component.onCompleted: {
    }


    ColumnLayout {
        //height: app_settings.menu_page_height
        width: app_settings.menu_page_width
        spacing: app_settings.column_layout_spacing

        Rectangle {
            height: app_settings.column_layout_spacing
            width: app_settings.menu_page_width
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
                        main_bar.on_switch_to_mode(app_settings.node_graph_mode)
                    } else {
                        file_page.update_current_graph(title_field.text, description_field.text)
                        main_bar.on_switch_to_mode(app_settings.file_mode)
                    }
                }
            }
            AppLabelButton {
                text: "cancel"
                onClicked: {
                    if (mode == "create") {
                        main_bar.on_switch_to_mode(app_settings.file_mode);
                    } else {
                        main_bar.on_switch_to_mode(app_settings.file_mode)
                    }
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
    }



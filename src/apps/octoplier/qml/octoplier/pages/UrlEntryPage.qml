import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.tools 1.0

Rectangle {
    id: url_entry_page

    // Dimensions.
    height: app_settings.menu_page_height
    width: app_settings.menu_page_width

    // Positioning.
    x: 0
    y: 0
    z: 0
    
    // Properties.
    color: app_settings.ng_bg_color

    ColumnLayout {
        //height: app_settings.menu_page_height
        width: app_settings.menu_page_width
        spacing: app_settings.column_layout_spacing

        Rectangle {
            height: app_settings.column_layout_spacing
            width: app_settings.menu_page_width
        }

        AppLabel {
            text: "URL"
        }

        // URL Text Entry Field.
        AppTextField {
            id: urlField
            tool_bar: copy_paste_bar
            //text: web_browser_page.web_view_alias.url
            inputMethodHints: Qt.ImhPreferLowercase | Qt.ImhUrlCharactersOnly
            text: "www."
            onAccepted: {
                main_bar.on_switch_to_mode(app_settings.web_browser_mode)
                web_browser_page.web_view_alias.url = utils.url_from_input(urlField.text)
            }
        }
        
        // Buttons.
        RowLayout {
            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "accept"
                onClicked: {
                    main_bar.switch_to_mode(app_settings.web_browser_mode);
                    web_browser_page.web_view_alias.url = utils.url_from_input(urlField.text)
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
                    main_bar.switch_to_mode(app_settings.web_browser_mode);
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}
    
    

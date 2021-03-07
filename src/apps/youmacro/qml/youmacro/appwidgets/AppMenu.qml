import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

Menu {
    title: "Some Kind of Menu"

    style: MenuStyle {
        id: desktop_menu_style

        // Frame around the menu.
        frame: Rectangle {
            color: app_settings.ng_bg_color
        }

        // The delegate representing each item in the menu.
        itemDelegate {
            background: Rectangle {
                color:  styleData.selected || styleData.open ? app_settings.tree_view_selected_row_bg_color : app_settings.ng_bg_color
            }

            label: Label {
                color: "white"
                text: styleData.text
            }

            submenuIndicator: Text {
                text: "\u25ba"
                color: "white"
            }

            shortcut: Label {
                color: "white"
                text: styleData.shortcut
            }
        }

        // The separator.
        separator: Rectangle {
            width: parent.width
            implicitHeight: app_units.dp(2)
            color: "white"
        }
    }
    
}
    

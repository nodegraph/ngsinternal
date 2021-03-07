import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import youmacro.appwidgets 1.0

Rectangle {
    id: select_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Appearance.
    color: app_settings.menu_stack_bg_color

    // Callback will be called with the selected option text.
    property var callback

    // Our Methods.
    function set_title(title) {
        stack_view_header.title_text = title
    }
    function get_title() {
        return stack_view_header.title_text
    }
    function set_option_texts(otexts) {
        combo_box.set_option_texts(otexts);
    }

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        stack_view: select_page.Stack.view
    }

    ColumnLayout {
        y: app_settings.action_bar_height

        //height: app_settings.menu_page_height
        width: app_settings.menu_page_width
        spacing: app_settings.column_layout_spacing

        AppSpacer {}

        AppComboBox {
            id: combo_box
            anchors {
			    left: parent.left
			    right: parent.right
			    leftMargin: app_settings.page_left_margin
			    rightMargin: app_settings.page_right_margin
			}
        }
        
        AppSpacer {}

        // Buttons.
        RowLayout {
        	Layout.maximumWidth: parent.width
        	
            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "accept"
                onClicked: {
                    var value = combo_box.currentText
                    select_page.callback(value)
                    select_page.Stack.view.pop_page()
                    main_bar.switch_to_last_mode()
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
                    select_page.Stack.view.pop_page()
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}



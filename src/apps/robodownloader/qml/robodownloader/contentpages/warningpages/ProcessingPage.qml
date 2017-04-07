import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import robodownloader.appwidgets 1.0

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

    // Our Methods.
    function set_title(title) {
        stack_view_header.title_text = title
    }
    function get_title() {
        return stack_view_header.title_text
    }
    
    function set_cancelable(c) {
    	stop_button.visible = c
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

        Rectangle {
            height: app_settings.column_layout_spacing
            width: app_settings.menu_page_width
            color: "transparent"
        }
        
        AppText  {
            id: description
            anchors.horizontalCenter: parent.horizontalCenter // used when the text is actually a single line
            Layout.maximumWidth: parent.width
            text: "The node graph is currently cleaning nodes."
        }

        // Buttons.
        RowLayout {
        	Layout.maximumWidth: parent.width
        	
            Item {Layout.fillWidth: true}
            AppLabelButton {
            	id: stop_button
                text: "stop cleaning"
                visible: true
                onClicked: {
                    manipulator.force_stack_reset()
                    page.Stack.view.pop_page()
                    main_bar.switch_to_current_mode()
                }
            }
            Rectangle {
                color: "transparent"
                height: app_settings.action_bar_height
                width: app_settings.button_spacing
            }
            AppLabelButton {
            	id: continue_button
                text: "continue cleaning"
                onClicked: {
                    page.Stack.view.pop_page()
                    main_bar.switch_to_current_mode()
                }
            }
            Item {Layout.fillWidth: true}
        }
    }
}



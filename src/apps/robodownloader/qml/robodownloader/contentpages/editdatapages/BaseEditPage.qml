import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
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
    property string description: ""
    property var exposable: page.Stack.view && (page.Stack.view.depth == 2)
    property Component value_editor_component
    property alias value_editor_loader: value_editor_loader
    
    // Our Methods.
    function set_value(value) {
        console.log("Error: BaseEditPage::set_value was not overridden.")
    }

    function get_value(value) {
    	console.log("Error: BaseEditPage::set_value was not overridden.")
        return 0
    }
    
    function set_exposed(exposed) {
    	expose_plug_check_box.set_exposed(exposed)
    }
    
    function get_exposed() {
    	return expose_plug_check_box.get_exposed()
    }

    function set_title(title) {
        stack_view_header.title_text = title
    }
    
    function get_title() {
        return stack_view_header.title_text
    }
    
    function set_description(desc) {
    	description.text = desc
    }

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        stack_view: page.Stack.view
    }

    ColumnLayout {
        id: column_layout
        y: app_settings.action_bar_height
        //height: app_settings.menu_page_height
        width: app_settings.menu_page_width
        spacing: app_settings.column_layout_spacing

        anchors {
            left: parent.left
            right: parent.right
            leftMargin: app_settings.page_left_margin
            rightMargin: app_settings.page_right_margin
        }

        AppSpacer {}
        
        AppText  {
            id: description
            anchors.horizontalCenter: parent.horizontalCenter // used when the text is actually a single line
            Layout.maximumWidth: parent.width
            text: "The current value of this property."
        }
        
        AppSpacer {}
        
        Loader {
            id: value_editor_loader
            sourceComponent: value_editor_component
        }
        
        AppSpacer {}
        
        AppExposePlugCheckBox {
            id: expose_plug_check_box
            visible: page.exposable
        }

		AppSpacer {}

        // Buttons.
        RowLayout {
        	Layout.maximumWidth: parent.width

            Item {Layout.fillWidth: true}
            AppLabelButton {
                text: "accept"
                onClicked: {
                    var path = page.Stack.view.get_title_path(1, page.Stack.view.depth)
                    page.Stack.view._stack_page.set_value(path, get_value())
                    if (page.exposable) {
                    	page.Stack.view._stack_page.set_exposed(path, get_exposed())
                    }
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



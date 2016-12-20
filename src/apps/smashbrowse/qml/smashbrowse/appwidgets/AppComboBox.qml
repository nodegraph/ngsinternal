import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0
import Qt.labs.settings 1.0

import smashbrowse.stackedmodepages 1.0
import smashbrowse.appwidgets 1.0

ComboBox {
    id: combo_box
	editable: false
	
    //anchors {
    //    left: parent.left
    //    right: parent.right
    //    leftMargin: app_settings.page_left_margin
    //    rightMargin: app_settings.page_right_margin
    //}
    
    function set_option_texts(otexts) {
        combo_box_model.clear()
        for (var i=0; i<otexts.length; i++) {
            combo_box_model.append({text: otexts[i]})
        }
    }
    
    model: ListModel {
        id: combo_box_model
    }
    
    style: ComboBoxStyle {
        id: comboBox
        background: Rectangle {
            radius: app_settings.check_box_radius
            border.width: app_settings.check_box_border_width
            color: app_settings.prompt_color
            border.color: 'white'
        }
        label: Text {
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: app_settings.font_point_size
            font.family: "Arial"
            font.capitalization: Font.MixedCase
            color: "white"
            text: control.currentText
        }
        // Dropdown.
        property Component __dropDownStyle: MenuStyle {
            __maxPopupHeight: app_settings.page_height
            __menuItemType: "comboboxitem"

            // Background.
            frame: Rectangle {
                radius: app_settings.check_box_radius
                border.width: app_settings.check_box_border_width
                color: app_settings.prompt_color
                border.color: 'white'
                width: combo_box.width - app_settings.page_right_margin
            }

            // Text.
            itemDelegate.label: Text {
                verticalAlignment: Text.AlignVCenter // These don't seem to work in a delegate.
                horizontalAlignment: Text.AlignHCenter // These don't seem to work in a delegate.
                font.pointSize: app_settings.font_point_size
                font.family: "Arial"
                font.capitalization: Font.MixedCase
                color: styleData.selected ? "white" : "white"
                text: styleData.text
                width: combo_box.width - app_settings.page_right_margin
                Layout.maximumWidth: parent.width
            }

            // Background Overlay.
            itemDelegate.background: Rectangle {
                radius: app_settings.check_box_radius
                color: styleData.selected ? app_settings.image_button_press_color : "transparent"
                width: combo_box.width - app_settings.page_right_margin
            }

            __scrollerStyle: ScrollViewStyle { }
        }
    }
}

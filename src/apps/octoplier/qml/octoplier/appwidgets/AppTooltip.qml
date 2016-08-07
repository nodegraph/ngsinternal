import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import octoplier.appwidgets 1.0

Rectangle {
    id: tooltip
    radius: app_units.dp(10)
    border {
        width: app_units.dp(3)
        color: "white"
    }
    
    // Public properties.
    property string tooltip_text: "Yay Tooltip!"
    
    function show(target,tip) {
        
        tooltip_text = tip
        visible = true
        
        var bottom_right = target.mapToItem(null, target.width, target.height)
        var bottom_left = target.mapToItem(null, 0, target.height)
        
        var left_space = bottom_left.x
        var right_space = app_window.width - bottom_right.x
        
        if (left_space > right_space) {
            y = bottom_left.y + target.height * 0.5
            x = bottom_left.x - tooltip.width
        } else {
            y = bottom_right.y + target.height * 0.5
            x = bottom_right.x
        }
        

    }

    function hide() {
        visible = false
    }
        
    // Dimensions.
    width: text_item.width + app_settings.action_bar_left_margin + app_settings.action_bar_right_margin
    height: app_settings.action_bar_height
    z: 20
    
    // Visibility.
    visible: false
    
    // Background color.
    color: app_settings.tooltip_bg_color
        
    // The text.
    Text {
        id: text_item
        anchors.centerIn: parent
        text: tooltip_text
        color: "white"
        font.pointSize: app_settings.font_point_size
    }

}

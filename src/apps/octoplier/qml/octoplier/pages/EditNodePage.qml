import QtQuick 2.6

Rectangle {
    
    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width
    
    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z
    
    // Appearance.
    color: "red"

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.edit_node_mode) {
            visible = true;
        } else {
            visible = false;
        }
    }
}

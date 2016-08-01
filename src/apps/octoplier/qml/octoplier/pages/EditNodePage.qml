import QtQuick 2.6

Rectangle {
    id: edit_node_page
    
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
            menu_stack_page.visible = false
        } else {
            visible = false;
        }
    }
}

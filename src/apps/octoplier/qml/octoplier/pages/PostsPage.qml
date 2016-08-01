import QtQuick 2.6
import NodeGraphTesting 1.0

import octoplier.appconfig 1.0
import octoplier.menus 1.0
import octoplier.appwidgets 1.0
import octoplier.menumodels 1.0

Rectangle {
    id: posts_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width
    
    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z
    
    // Appearance.
    color: app_settings.ng_bg_color
    
    // Methods.
    function on_switch_to_mode(mode) {
    	if (mode == app_settings.posts_mode) {
            visible = true;
            menu_stack_page.visible = false
        } else {
        	visible = false;
        }
    }
}

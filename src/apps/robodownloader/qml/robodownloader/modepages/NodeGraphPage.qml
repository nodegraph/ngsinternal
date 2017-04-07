import QtQuick 2.7

import robodownloader.appwidgets 1.0

Rectangle {
    id: node_graph_page
    objectName: "node_graph_page"

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    // Appearance.
    color: app_settings.ng_bg_color
    opacity: 1

    // Signals.
    signal popup_menu()
    signal popup_menu_with_centering()

    // Methods.
    function show_page() {
    	visible = true
    	node_graph_item.forceActiveFocus() 
    }
    function hide_page() {
    	visible = false
    }
    function on_switch_to_mode(mode) {
        if (mode == app_settings.node_graph_mode) {
            show_page()
        } else {
            hide_page()
        }
    }
    
    Keys.onPressed: {
		console.log('node graph page key pressed')
    }
    Keys.onReleased: {
        console.log('node graph page key released')
    }
    
}


import QtQuick 2.6
//import NodeGraphRendering 1.0

Rectangle {
    id: node_graph_page
    objectName: "node_graph_page"

    //property alias node_graph: our_node_graph
    property var node_graph

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
    function on_switch_to_mode(mode) {
        if (mode == app_settings.node_graph_mode) {
            visible = true
        } else {
            visible = false
        }
    }
}


import QtQuick 2.6
import NodeGraphRendering 1.0

Rectangle {
    id: node_graph_page
    property alias node_graph: our_node_graph

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    //anchors.fill: parent

    // Appearance.
    color: "green"
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

    // The node graph.
    NodeGraphQuickItem {
        id: our_node_graph
        objectName: "node_graph_object"
        anchors.fill: node_graph_page
        //anchors.margins: 0
        //opacity: 0.5
    }
}


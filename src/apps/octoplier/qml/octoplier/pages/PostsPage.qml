import QtQuick 2.4
import NodeGraphTesting 1.0

Rectangle {
    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width
    
    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z
    
    // Appearance.
    color: "yellow"
    
    // Methods.
    function on_switch_to_mode(mode) {
    	if (mode == app_settings.posts_mode) {
            visible = true;
        } else {
        	visible = false;
        }
    }

    //function build_test_graph() {
    //    our_node_renderer.build_test_graph()
    //}


    // The node graph.
    //NodeGraphRenderer {
    //    id: our_node_renderer
    //    objectName: "node_graph_object"
    //    anchors.fill: parent
    //    //anchors.margins: 0
    //    //opacity: 0.5
    //}
}

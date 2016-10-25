import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Data Group Node"
        description: "A node which groups data processing nodes inside of itself."
        script: "node_graph_item.create_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Script Group Node"
        description: "A node which uses a script to compute its outputs. Input and Output nodes define the interface to the script."
        script: "node_graph_item.create_script_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Web Group Node"
        description: "A node which groups web browser action nodes inside of itself."
        script: "node_graph_item.create_web_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Data Node"
        description: "A node which emits user specified data from its output plug."
        script: "node_graph_item.create_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Input Node"
        description: "A node which relays data from outside a group to inside the group."
        script: "node_graph_item.create_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_crop_square_white_48dp.png"
        title: "Output Node"
        description: "A node which relays data from inside a group to outside the group."
        script: "node_graph_item.create_output_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_fiber_manual_record_white_48dp.png"
        title: "Dot Node"
        description: "A nodes which allows you to route links, in order to prevent overlaps."
        script: "node_graph_item.create_dot_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "Mock Node"
        description: "Add node which is used for testing."
        script: "node_graph_item.create_mock_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }


    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "Open Browser"
        description: "A node which opens the browser."
        script: "node_graph_item.create_open_browser_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "Close Browser"
        description: "A node which close the browser."
        script: "node_graph_item.create_close_browser_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "Create Set From Values"
        description: "A node which creates an element set in the browser by matching text or images."
        script: "node_graph_item.create_create_set_from_values_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "Create Set From Type"
        description: "A node which creates an element set in the browser by matching the type of the element."
        script: "node_graph_item.create_create_set_from_type_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

}

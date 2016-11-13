import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Data Group Node"
        description: "Encapsulates data processing logic."
        script: "node_graph_item.create_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Browser Group Node"
        description: "Encapsulates web browser manipulation logic."
        script: "node_graph_item.create_browser_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Script Group Node"
        description: "Evaluates a script to compute the groups outputs. Input and Output nodes define the interface to the script."
        script: "node_graph_item.create_script_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Firebase Group Node"
        description: "Provides an environment where you manipulate Firebase databases."
        script: "node_graph_item.create_firebase_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "MQTT Group Node"
        description: "Provides an environment where you can publish and subscribe to MQTT topis and messages."
        script: "node_graph_item.create_mqtt_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Data Node"
        description: "Outputs user specified data from its output plug."
        script: "node_graph_item.create_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Input Node"
        description: "Relays data from outside a group to inside the group."
        script: "node_graph_item.create_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_crop_square_white_48dp.png"
        title: "Output Node"
        description: "Relays data from inside a group to outside the group."
        script: "node_graph_item.create_output_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_fiber_manual_record_white_48dp.png"
        title: "Dot Node"
        description: "Allows you to bend links to decrease link crossings."
        script: "node_graph_item.create_dot_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "Merge Node"
        description: "Merges data from two inputs and outputs it."
        script: "node_graph_item.create_merge_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "Firebase Write Data"
        description: "Merges data from two inputs and outputs it."
        script: "node_graph_item.create_firebase_write_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "Firebase Read Data"
        description: "Merges data from two inputs and outputs it."
        script: "node_graph_item.create_firebase_read_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
	ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "HTTP"
        description: "Make an HTTP request."
        script: "node_graph_item.create_http_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "MQTT Publish"
        description: "Publish a message to a certain topic."
        script: "node_graph_item.create_mqtt_publish_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "MQTT Subscribe"
        description: "Subscribe to a topic."
        script: "node_graph_item.create_mqtt_subscribe_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

}

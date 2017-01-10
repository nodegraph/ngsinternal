import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Macro Node"
        description: "Testing."
        script: "ng_controller.create_macro_node(center_new_nodes, 'untitled');
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Group Node"
        description: "Encapsulates data processing logic."
        script: "ng_controller.create_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "If Group Node"
        description: "This group computes only when the value at condition_path evaluates to true."
        script: "ng_controller.create_if_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "While Group Node"
        description: "This group computes only while the value at condition_path remains true."
        script: "ng_controller.create_while_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "For Each Group Node"
        description: "This group computes once for every element in the elements input."
        script: "ng_controller.create_for_each_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Browser Group Node"
        description: "Encapsulates web browser manipulation logic."
        script: "ng_controller.create_browser_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Script Group Node"
        description: "Evaluates a script to compute the groups outputs. Input and Output nodes define the interface to the script."
        script: "ng_controller.create_script_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Script Node"
        description: "Uses script to transform the input object into the output object."
        script: "ng_controller.create_script_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Firebase Group Node"
        description: "Provides an environment where you manipulate Firebase databases."
        script: "ng_controller.create_firebase_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "MQTT Group Node"
        description: "Provides an environment where you can publish and subscribe to MQTT topis and messages."
        script: "ng_controller.create_mqtt_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Accumulate Data Node"
        description: "Merges all input values passing through this node."
        script: "ng_controller.create_accumulate_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Copy Data Node"
        description: "Copies data inside the input object to another location."
        script: "ng_controller.create_copy_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Erase Data Node"
        description: "Erases data a certain location inside the input object."
        script: "ng_controller.create_erase_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Number Data Node"
        description: "Outputs a number value from its output plug."
        script: "ng_controller.create_number_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "String Data Node"
        description: "Outputs a string value from its output plug."
        script: "ng_controller.create_string_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Boolean Data Node"
        description: "Outputs a boolean value from its output plug."
        script: "ng_controller.create_boolean_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Array Data Node"
        description: "Outputs an array value from its output plug."
        script: "ng_controller.create_array_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Object Data Node"
        description: "Outputs an object value from its output plug."
        script: "ng_controller.create_object_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    
    
    
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Number Input Node"
        description: "Relays data from outside a group to inside the group."
        script: "ng_controller.create_number_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "String Input Node"
        description: "Relays data from outside a group to inside the group."
        script: "ng_controller.create_string_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Boolean Input Node"
        description: "Relays data from outside a group to inside the group."
        script: "ng_controller.create_boolean_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Array Input Node"
        description: "Relays data from outside a group to inside the group."
        script: "ng_controller.create_array_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_details_white_48dp.png"
        title: "Object Input Node"
        description: "Relays data from outside a group to inside the group."
        script: "ng_controller.create_object_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_crop_square_white_48dp.png"
        title: "Output Node"
        description: "Relays data from inside a group to outside the group."
        script: "ng_controller.create_output_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_fiber_manual_record_white_48dp.png"
        title: "Dot Node"
        description: "Allows you to bend links to decrease link crossings."
        script: "ng_controller.create_dot_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_fiber_manual_record_white_48dp.png"
        title: "Wait Node"
        description: "Pauses the processing of the graph for the specified amount of time."
        script: "ng_controller.create_wait_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "Merge Node"
        description: "Merges data from two inputs and outputs it."
        script: "ng_controller.create_merge_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "Firebase Write Data"
        description: "Merges data from two inputs and outputs it."
        script: "ng_controller.create_firebase_write_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "Firebase Read Data"
        description: "Merges data from two inputs and outputs it."
        script: "ng_controller.create_firebase_read_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
	ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "HTTP"
        description: "Make an HTTP request."
        script: "ng_controller.create_http_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "MQTT Publish"
        description: "Publish a message to a certain topic."
        script: "ng_controller.create_mqtt_publish_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_gesture_white_48dp.png"
        title: "MQTT Subscribe"
        description: "Subscribe to a topic."
        script: "ng_controller.create_mqtt_subscribe_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

}

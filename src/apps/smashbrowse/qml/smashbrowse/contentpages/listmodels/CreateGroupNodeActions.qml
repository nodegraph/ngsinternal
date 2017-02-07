import QtQuick 2.6

ListModel {
    
    ListElement {
    	letters: "Gp"
        title: "Group Node"
        description: "Group child nodes under one node."
        script: "ng_controller.create_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
    	letters: "If"
        title: "If Group Node"
        description: "This group runs when the property at condition_path evaluates to true."
        script: "ng_controller.create_if_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
    	letters: "Wl"
        title: "While Group Node"
        description: "This group runs while the property at condition_path remains true."
        script: "ng_controller.create_while_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
    	letters: "Fo"
        title: "For Each Group Node"
        description: "This group runs once for every element in the elements input."
        script: "ng_controller.create_for_each_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
    	letters: "Br"
        title: "Browser Group Node"
        description: "This group allows creation of web browser nodes."
        script: "ng_controller.create_browser_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
    	letters: "Sg"
        title: "Script Group Node"
        description: "A node which runs a script. The Input and Output nodes define the interface to the script."
        script: "ng_controller.create_script_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    //ListElement {
    //	letters: "Fb"
    //    title: "Firebase Group Node"
    //    description: "Provides an environment where you manipulate Firebase databases."
    //    script: "ng_controller.create_firebase_group_node(center_new_nodes);
    //             main_bar.switch_to_mode(app_settings.node_graph_mode);"
    //}
    
    ListElement {
    	letters: "Mq"
        title: "MQTT Group Node"
        description: "This group allows creations of nodes to publish and subscribe to MQTT topics."
        script: "ng_controller.create_mqtt_group_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
}
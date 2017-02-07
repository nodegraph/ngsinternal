import QtQuick 2.6

ListModel {
    //ListElement {
    //	letters: "Fw"
    //    title: "Firebase Write Data"
    //    description: "Write data to a Firebase."
    //    script: "ng_controller.create_firebase_write_data_node(center_new_nodes);
    //             main_bar.switch_to_mode(app_settings.node_graph_mode);"
    //}
    //ListElement {
    //	letters: "Fr"
    //    title: "Firebase Read Data"
    //    description: "Read data from Firebase."
    //    script: "ng_controller.create_firebase_read_data_node(center_new_nodes);
    //             main_bar.switch_to_mode(app_settings.node_graph_mode);"
    //}
    
	ListElement {
		letters: "Hp"
        title: "HTTP"
        description: "Make an HTTP request."
        script: "ng_controller.create_http_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
    	letters: "Mp"
        title: "MQTT Publish"
        description: "Publish a message to a certain topic using MQTT."
        script: "ng_controller.create_mqtt_publish_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
    ListElement {
    	letters: "Ms"
        title: "MQTT Subscribe"
        description: "Subscribe to a topic using MQTT."
        script: "ng_controller.create_mqtt_subscribe_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
}
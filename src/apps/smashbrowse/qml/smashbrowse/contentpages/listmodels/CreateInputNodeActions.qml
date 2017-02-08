import QtQuick 2.6

ListModel {

	ListElement {
    	letters: "Ni"
        title: "Number Input Node"
        description: "Adds a number input (param) to the group."
        script: "ng_controller.create_number_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Si"
        title: "String Input Node"
        description: "Adds a string input (param) to the group."
        script: "ng_controller.create_string_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Bi"
        title: "Boolean Input Node"
        description: "Adds a boolean input (param) to the group."
        script: "ng_controller.create_boolean_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Ai"
        title: "Array Input Node"
        description: "Adds an array input (param) to the group."
        script: "ng_controller.create_array_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Oi"
        title: "Adds an object input (param) to the group."
        description: "Relays data from outside a group to inside the group."
        script: "ng_controller.create_object_input_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

}
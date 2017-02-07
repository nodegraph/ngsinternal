import QtQuick 2.6

ListModel {

    ListElement {
    	letters: "Nm"
        title: "Number Data Node"
        description: "Outputs a number value."
        script: "ng_controller.create_number_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "St"
        title: "String Data Node"
        description: "Outputs a string value."
        script: "ng_controller.create_string_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Bl"
        title: "Boolean Data Node"
        description: "Outputs a boolean value."
        script: "ng_controller.create_boolean_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Ar"
        title: "Array Data Node"
        description: "Outputs an array value."
        script: "ng_controller.create_array_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Ob"
        title: "Object Data Node"
        description: "Outputs an object value."
        script: "ng_controller.create_object_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

}
import QtQuick 2.7
import GUITypes 1.0

ListModel {

	ListElement {
    	letters: "Pi"
        title: "Password Input Node"
        description: "Adds a password input (param) to the group."
        script: "ng_controller.create_password_input_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
    ListElement {
    	letters: "Si"
        title: "String Input Node"
        description: "Adds a string input (param) to the group."
        script: "ng_controller.create_string_input_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
    ListElement {
    	letters: "Ni"
        title: "Number Input Node"
        description: "Adds a number input (param) to the group."
        script: "ng_controller.create_number_input_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
    ListElement {
    	letters: "Bi"
        title: "Boolean Input Node"
        description: "Adds a boolean input (param) to the group."
        script: "ng_controller.create_boolean_input_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
    ListElement {
    	letters: "Ai"
        title: "Array Input Node"
        description: "Adds an array input (param) to the group."
        script: "ng_controller.create_array_input_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
    ListElement {
    	letters: "Oi"
        title: "Object Input Node"
        description: "Adds an object input (param) to the group."
        script: "ng_controller.create_object_input_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }

}
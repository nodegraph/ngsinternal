import QtQuick 2.7
import GUITypes 1.0

ListModel {

	ListElement {
    	letters: "Dt"
        title: "Dot Node"
        description: "Passes data through untouched. Used to visually organize links."
        script: "ng_controller.create_dot_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
    ListElement {
    	letters: "Wt"
        title: "Wait Node"
        description: "Pauses the processing of the graph for the specified amount of time."
        script: "ng_controller.create_wait_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
    ListElement {
    	letters: "Mg"
        title: "Merge Node"
        description: "Merges data from two inputs."
        script: "ng_controller.create_merge_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }

    ListElement {
    	letters: "Sr"
        title: "Script Node"
        description: "Transform the input data using a script."
        script: "ng_controller.create_script_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	letters: "Xp"
        title: "Extract Property Node"
        description: "Extract a property from within the input data."
        script: "ng_controller.create_extract_property_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	letters: "Mp"
        title: "Embed Property Node"
        description: "Embed the value property to another location in the output data."
        script: "ng_controller.create_embed_property_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	letters: "Ep"
        title: "Erase Property Node"
        description: "Erase the specified property in the output data."
        script: "ng_controller.create_erase_property_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
    
    //ListElement {
    //	letters: "Cp"
    //    image_url: "qrc:///icons/ic_details_white_48dp.png"
    //    title: "Copy Data Node"
    //    description: "Copies a property in the data to another location."
    //    script: "ng_controller.create_copy_data_node(center_new_nodes);
    //             main_bar.switch_to_last_mode();"
    //}
    
    //ListElement {
    //	letters: "Er"
    //    title: "Erase Data Node"
    //    description: "Erases a property in the data."
    //    script: "ng_controller.create_erase_data_node(center_new_nodes);
    //             main_bar.switch_to_last_mode();"
    //}
    
    

	ListElement {
    	letters: "Po"
        title: "Post Data Node"
        description: "Posts a property a property from the input data to the posts page."
        script: "ng_controller.create_post_node(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }



}
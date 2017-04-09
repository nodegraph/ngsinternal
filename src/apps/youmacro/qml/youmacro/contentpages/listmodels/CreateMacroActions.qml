import QtQuick 2.7

ListModel {

    ListElement {
    	letters: "Dv"
        title: "YouTube - Download Vlogs"
        description: "Download the latest vlogs using the vlogger's name."
        script: '_stack_page.create_macro("gmail_login", "GMail Login", "Log into GMail account.");'
    }
    ListElement {
    	letters: "Dt"
        title: "YouTube - Download Trending Videos"
        description: "Download the latest trending videos."
        script: "ng_controller.create_string_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Pr"
        title: "YouTube - Play Random Videos"
        description: "Play random videos"
        script: "ng_controller.create_number_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Dc"
        title: "Vimeo - Download Creator Videos"
        description: "Download the latest videos using the creator's name"
        script: "ng_controller.create_number_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Pv"
        title: "Vimeo - Download Trending Videos"
        description: "Download the latest trending videos."
        script: "ng_controller.create_number_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Pv"
        title: "Vimeo - Play Random Videos"
        description: "Play Random videos."
        script: "ng_controller.create_number_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Gl"
        title: "GMail - Login"
        description: "Login into your GMail account."
        script: "ng_controller.create_boolean_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Ol"
        title: "Outlook - Login"
        description: "Login to your Outlook account."
        script: "ng_controller.create_array_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
    	letters: "Yl"
        title: "Yahoo - Login"
        description: "Login to your Yahoo account."
        script: "ng_controller.create_object_data_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

}
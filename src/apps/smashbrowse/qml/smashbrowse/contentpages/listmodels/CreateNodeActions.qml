import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

	ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Group Nodes..."
        description: "Group nodes encapsulate child nodes under one node."
        script: "push_by_names('group nodes', 'NodeMenuListPage', 'CreateGroupNodeActions')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_filter_1_white_48dp.png"
        title: "Data Nodes..."
        description: "Data nodes introduce data into graph flow."
        script: "push_by_names('data nodes', 'NodeMenuListPage', 'CreateDataNodeActions')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_crop_square_white_48dp.png"
        title: "Input Nodes..."
        description: "Relays data from outside the group to the inside."
        script: "push_by_names('group nodes', 'NodeMenuListPage', 'CreateInputNodeActions')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_photo_filter_white_48dp.png"
        title: "Data Op Nodes..."
        description: "Manipulate data flowing through the links."
        script: "push_by_names('group nodes', 'NodeMenuListPage', 'CreateDataOpNodeActions')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_graphic_eq_white_48dp.png"
        title: "Protocol Nodes..."
        description: "Talk to external services using protocols like MQTT."
        script: "push_by_names('group nodes', 'NodeMenuListPage', 'CreateProtocolNodeActions')"
    }
    
    ListElement {
    	image_url: "qrc:///icons/ic_details_white_48dp.png"
    	letters: "Ot"
        title: "Output Node"
        description: "Relays data from inside the group to the outside."
        script: "ng_controller.create_output_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    
	ListElement {
    	image_url: "qrc:///icons/ic_details_white_48dp.png"
    	letters: "Er"
        title: "Error Node"
        description: "When this node is reached an error is raised."
        script: "ng_controller.create_error_node(center_new_nodes);
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }


}

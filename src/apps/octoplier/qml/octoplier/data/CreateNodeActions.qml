import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {
    ListElement {
        navigator_title: "Create Node"

        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Group Node"
        description: "A node which groups a number of nodes inside of itself."
        script: "node_graph_page.node_graph.create_group_node(center_new_nodes)"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Input Node"
        description: "A node which relays data from outside a group to inside the group."
        script: "node_graph_page.node_graph.create_input_node(center_new_nodes)"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Output Node"
        description: "A node which relays data from inside a group to outside the group."
        script: "node_graph_page.node_graph.create_output_node(center_new_nodes)"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Dot Node"
        description: "A nodes which allows you to route links, in order to prevent overlaps."
        script: "node_graph_page.node_graph.create_dot_node(center_new_nodes)"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Mock Node"
        description: "Add node which is used for testing."
        script: "node_graph_page.node_graph.create_mock_node(center_new_nodes)"
    }
}

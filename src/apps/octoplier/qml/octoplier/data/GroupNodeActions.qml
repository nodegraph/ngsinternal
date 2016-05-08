import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {
    ListElement {
        navigator_title: "Group Node Actions"
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "View"
        description: "View the output data from the group node."
        next_model: "CreateNodeActions"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Edit"
        description: "Edit the parameters of the group node"
        next_model: "CreateNodeActions"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Toggle Selected State"
        description: "Toggle the selected state of the group node."
        script: "node_graph_page.node_graph.toggle_selection_under_long_press()"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Collapse To Group"
        description: "Collapses currently selected nodes to a new group node."
        script: "node_graph_page.node_graph.collapse_to_group()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Dive"
        description: "Dive into the group node."
        script: "node_graph_page.node_graph.dive();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Explode"
        description: "Explode the group node."
        script: "node_graph_page.node_graph.explode_group()"
    }
}

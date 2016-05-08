import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {
    ListElement {
        navigator_title: "Explode Collapse Nodes"

        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Collapse"
        description: "Collapse selected nodes into a group node."
        script: "node_graph_page.node_graph.collapse_to_group()"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Explode"
        description: "Explode the selected group node out into the current group."
        script: "node_graph_page.node_graph.explode_group()"
    }
}


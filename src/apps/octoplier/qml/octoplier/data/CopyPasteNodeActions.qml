import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {
    ListElement {
        navigator_title: "Copy Paste Nodes"

        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Copy"
        description: "Copy all the selected nodes."
        script: "node_graph_page.node_graph.copy()"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Cut"
        description: "Cut all the selected nodes."
        script: "node_graph_page.node_graph.cut()"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Paste"
        description: "Paste nodes copied from the last copy."
        script: "node_graph_page.node_graph.paste(center_new_nodes)"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Delete"
        description: "Delete the selected nodes."
        script: "node_graph_page.node_graph.destroy_selection()"
    }
}


import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {
    ListElement {
        navigator_title: "Frame Nodes"
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Frame All Nodes"
        description: "Frame all nodes, so that they all fit on the screen."
        script: "node_graph_page.node_graph.frame_all()"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Frame Selected Nodes"
        description: "Frame selected nodes, so that they all fit on the screen."
        script: "node_graph_page.node_graph.frame_selected()"
    }
}
